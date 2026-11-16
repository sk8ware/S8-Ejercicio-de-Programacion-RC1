#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_PRODUCTOS 5
#define MAX_NOMBRE 40
#define MAX_DEMANDA 100000
#define MAX_POR_UNIDAD 100000
#define MAX_CAPACIDAD 1000000000000LL

void leerLinea(char linea[], int capacidad) {
    int c;
    size_t largo;
    for (;;) {
        if (fgets(linea, capacidad, stdin) == NULL) {
            printf("\nFin de entrada.\n");
            exit(0);
        }
        largo = strlen(linea);
        if (largo > 0 && linea[largo - 1] == '\n') {
            linea[largo - 1] = '\0';
            return;
        }
    
        if (!feof(stdin)) {
            c = getchar();
            if (c != '\n' && c != EOF) {
                while ((c = getchar()) != '\n' && c != EOF) { }
                printf("Entrada demasiado larga. Intente otra vez.\n");
                continue;
            }
        }
        return;
    }
}

long long leerEntero(const char etiqueta[], long long minimo, long long maximo) {
    char linea[128], *fin;
    long long numero;
    for (;;) {
        printf("%s", etiqueta);
        leerLinea(linea, sizeof linea);
        errno = 0;
        numero = strtoll(linea, &fin, 10);
        if (linea == fin || errno == ERANGE) {
            printf("Error: escriba un numero entero valido.\n");
            continue;
        }
        while (*fin != '\0' && isspace((unsigned char)*fin)) fin++;
        if (*fin != '\0' || numero < minimo || numero > maximo) {
            printf("Error: valor fuera de rango [%lld, %lld].\n", minimo, maximo);
            continue;
        }
        return numero;
    }
}

void leerNombre(char nombre[MAX_NOMBRE]) {
    int valido;
    do {
        printf("Nombre del producto (1-39 caracteres): ");
        leerLinea(nombre, MAX_NOMBRE);
        valido = 0;
        for (int i = 0; nombre[i] != '\0'; i++) {
            if (!isspace((unsigned char)nombre[i])) valido = 1;
        }
        if (!valido) puts("Error: el nombre no puede estar vacio.");
    } while (!valido);
}

int nombresIguales(const char a[], const char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) return 0;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

int buscarIndice(char nombres[][MAX_NOMBRE], int cantidad, const char nombre[]) {
    for (int i = 0; i < cantidad; i++) {
        if (nombresIguales(nombres[i], nombre)) return i;
    }
    return -1;
}

void registrar(char nombres[][MAX_NOMBRE], int demanda[], int minutos[],
              int recursos[], int *cantidad) {
    char nombre[MAX_NOMBRE];
    if (*cantidad >= MAX_PRODUCTOS) {
        puts("No se puede registrar: limite de cinco productos alcanzado.");
        return;
    }
    leerNombre(nombre);
    if (buscarIndice(nombres, *cantidad, nombre) != -1) {
        puts("Error: ya existe un producto con ese nombre.");
        return;
    }
    strcpy(nombres[*cantidad], nombre);
    demanda[*cantidad] = (int)leerEntero("Demanda (unidades, 0-100000): ", 0, MAX_DEMANDA);
    minutos[*cantidad] = (int)leerEntero("Minutos de fabricacion por unidad: ", 1, MAX_POR_UNIDAD);
    recursos[*cantidad] = (int)leerEntero("Recursos por unidad: ", 1, MAX_POR_UNIDAD);
    (*cantidad)++;
    puts("Producto registrado correctamente.");
}

void listar(char nombres[][MAX_NOMBRE], int demanda[], int minutos[],
            int recursos[], int cantidad) {
    if (cantidad == 0) { puts("No hay productos registrados."); return; }
    puts("\nPRODUCTOS REGISTRADOS");
    puts("No. | Nombre                | Demanda | Min/u | Recurso/u");
    puts("----+-----------------------+---------+-------+----------");
    for (int i = 0; i < cantidad; i++) {
        printf("%3d | %-21.21s | %7d | %5d | %9d\n", i + 1,
               nombres[i], demanda[i], minutos[i], recursos[i]);
    }
}

int solicitarIndice(char nombres[][MAX_NOMBRE], int cantidad) {
    char nombre[MAX_NOMBRE];
    int posicion;
    if (cantidad == 0) { puts("No hay productos registrados."); return -1; }
    leerNombre(nombre);
    posicion = buscarIndice(nombres, cantidad, nombre);
    if (posicion == -1) puts("Producto no encontrado.");
    return posicion;
}

void buscarProducto(char nombres[][MAX_NOMBRE], int demanda[], int minutos[],
                   int recursos[], int cantidad) {
    int i = solicitarIndice(nombres, cantidad);
    if (i == -1) return;
    printf("Encontrado: %s | Demanda: %d | Min/u: %d | Recurso/u: %d\n",
           nombres[i], demanda[i], minutos[i], recursos[i]);
}

void editar(char nombres[][MAX_NOMBRE], int demanda[], int minutos[],
            int recursos[], int cantidad) {
    int i = solicitarIndice(nombres, cantidad);
    char nuevo[MAX_NOMBRE];
    int existe;
    if (i == -1) return;
    puts("Ingrese los nuevos datos del producto:");
    for (;;) {
        leerNombre(nuevo);
        existe = buscarIndice(nombres, cantidad, nuevo);
        if (existe == -1 || existe == i) break;
        puts("Error: ya hay otro producto con ese nombre.");
    }
    strcpy(nombres[i], nuevo);
    demanda[i] = (int)leerEntero("Nueva demanda (0-100000): ", 0, MAX_DEMANDA);
    minutos[i] = (int)leerEntero("Nuevos minutos por unidad: ", 1, MAX_POR_UNIDAD);
    recursos[i] = (int)leerEntero("Nuevos recursos por unidad: ", 1, MAX_POR_UNIDAD);
    puts("Producto actualizado correctamente.");
}

void eliminar(char nombres[][MAX_NOMBRE], int demanda[], int minutos[],
              int recursos[], int *cantidad) {
    int i = solicitarIndice(nombres, *cantidad);
    if (i == -1) return;
    for (int j = i; j < *cantidad - 1; j++) {
        strcpy(nombres[j], nombres[j + 1]);
        demanda[j] = demanda[j + 1];
        minutos[j] = minutos[j + 1];
        recursos[j] = recursos[j + 1];
    }
    (*cantidad)--;
    puts("Producto eliminado correctamente.");
}

void configurar(long long *tiempoDisponible, long long *stockDisponible) {
    *tiempoDisponible = leerEntero("Tiempo disponible (minutos): ", 0, MAX_CAPACIDAD);
    *stockDisponible = leerEntero("Recursos disponibles (unidades): ", 0, MAX_CAPACIDAD);
    puts("Limites guardados en memoria.");
}

void evaluar(char nombres[][MAX_NOMBRE], int demanda[], int minutos[],
             int recursos[], int cantidad, long long tiempoDisponible,
             long long stockDisponible) {
    long long tiempoTotal = 0, recursosTotales = 0;
    if (cantidad == 0) { puts("Registre productos antes de evaluar."); return; }
    puts("\nREPORTE DE PRODUCCION");
    puts("Producto             | Tiempo total (min) | Recursos totales");
    puts("---------------------+--------------------+-----------------");
    for (int i = 0; i < cantidad; i++) {
        long long tiempoProducto = (long long)demanda[i] * minutos[i];
        long long recursosProducto = (long long)demanda[i] * recursos[i];
        tiempoTotal += tiempoProducto;
        recursosTotales += recursosProducto;
        printf("%-20.20s | %18lld | %16lld\n", nombres[i],
               tiempoProducto, recursosProducto);
    }
    printf("TOTAL TIEMPO REQUERIDO: %lld minutos\n", tiempoTotal);
    printf("TOTAL RECURSOS REQUERIDOS: %lld unidades\n", recursosTotales);
    printf("TIEMPO DISPONIBLE: %lld minutos\n", tiempoDisponible);
    printf("RECURSOS DISPONIBLES: %lld unidades\n", stockDisponible);
    printf("EVALUACION: %s\n", (tiempoTotal <= tiempoDisponible &&
                                     recursosTotales <= stockDisponible)
                                    ? "SI puede cumplir la demanda" :
                                      "NO puede cumplir la demanda");
    if (tiempoTotal > tiempoDisponible)
        printf("DEFICIT DE TIEMPO: %lld minutos\n", tiempoTotal - tiempoDisponible);
    if (recursosTotales > stockDisponible)
        printf("DEFICIT DE RECURSOS: %lld unidades\n", recursosTotales - stockDisponible);
}

int main(void) {
    char nombres[MAX_PRODUCTOS][MAX_NOMBRE] = {{0}};
    int demanda[MAX_PRODUCTOS] = {0};
    int minutos[MAX_PRODUCTOS] = {0};
    int recursos[MAX_PRODUCTOS] = {0};
    int cantidad = 0, opcion;
    long long tiempoDisponible = 0, stockDisponible = 0;
    puts("SISTEMA DE CONTROL DE PRODUCCION - RC1");
    puts("Configure primero los limites mediante la opcion 6.");
    do {
        puts("\n--- MENU PRINCIPAL ---");
        puts("1. Registrar producto");
        puts("2. Listar productos");
        puts("3. Buscar producto");
        puts("4. Editar producto");
        puts("5. Eliminar producto");
        puts("6. Configurar limites de produccion");
        puts("7. Calcular necesidades y factibilidad");
        puts("0. Salir");
        opcion = (int)leerEntero("Seleccione una opcion: ", 0, 7);
        switch (opcion) {
            case 1: registrar(nombres, demanda, minutos, recursos, &cantidad); break;
            case 2: listar(nombres, demanda, minutos, recursos, cantidad); break;
            case 3: buscarProducto(nombres, demanda, minutos, recursos, cantidad); break;
            case 4: editar(nombres, demanda, minutos, recursos, cantidad); break;
            case 5: eliminar(nombres, demanda, minutos, recursos, &cantidad); break;
            case 6: configurar(&tiempoDisponible, &stockDisponible); break;
            case 7: evaluar(nombres, demanda, minutos, recursos, cantidad,
                            tiempoDisponible, stockDisponible); break;
            case 0: puts("Programa finalizado."); break;
        }
    } while (opcion != 0);
    return 0;
}
