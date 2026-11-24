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

int leerLinea(char linea[], int capacidad) {
    int c;
    int largo;

    if (fgets(linea, capacidad, stdin) == NULL) {
        printf("\nFin de entrada.\n");
        exit(0);
    }

    largo = strlen(linea);

    if (largo > 0 && linea[largo - 1] == '\n') {
        linea[largo - 1] = '\0';
        return 1;
    }

    c = getchar();

    if (c == '\n' || c == EOF) {
        return 1;
    }

    while (c != '\n' && c != EOF) {
        c = getchar();
    }

    printf("Error: entrada demasiado larga.\n");
    return 0;
}

long long leerEntero(const char etiqueta[], long long minimo, long long maximo) {
    char linea[128];
    char *fin;
    long long numero;

    while (1) {
        printf("%s", etiqueta);

        if (leerLinea(linea, sizeof(linea)) == 0) {
            continue;
        }

        errno = 0;
        numero = strtoll(linea, &fin, 10);

        if (linea == fin || errno == ERANGE) {
            printf("Error: ingrese un numero entero valido.\n");
            continue;
        }

        while (*fin != '\0' && isspace((unsigned char)*fin)) {
            fin++;
        }

        if (*fin != '\0') {
            printf("Error: no ingrese letras ni decimales.\n");
            continue;
        }

        if (numero < minimo || numero > maximo) {
            printf("Error: ingrese un valor entre %lld y %lld.\n",
                   minimo, maximo);
            continue;
        }

        return numero;
    }
}

void quitarEspacios(char nombre[]) {
    int inicio = 0;
    int fin = strlen(nombre);
    int i;
    int j = 0;

    while (nombre[inicio] != '\0' &&
           isspace((unsigned char)nombre[inicio])) {
        inicio++;
    }

    while (fin > inicio &&
           isspace((unsigned char)nombre[fin - 1])) {
        fin--;
    }

    for (i = inicio; i < fin; i++) {
        nombre[j] = nombre[i];
        j++;
    }

    nombre[j] = '\0';
}

void leerNombre(char nombre[MAX_NOMBRE]) {
    while (1) {
        printf("Nombre del producto (1-39 caracteres): ");

        if (leerLinea(nombre, MAX_NOMBRE) == 0) {
            continue;
        }

        quitarEspacios(nombre);

        if (strlen(nombre) == 0) {
            printf("Error: el nombre no puede estar vacio.\n");
        } else {
            return;
        }
    }
}

int nombresIguales(const char a[], const char b[]) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (tolower((unsigned char)a[i]) !=
            tolower((unsigned char)b[i])) {
            return 0;
        }

        i++;
    }

    if (a[i] == '\0' && b[i] == '\0') {
        return 1;
    }

    return 0;
}

int buscarIndice(char nombres[][MAX_NOMBRE], int cantidad,
                 const char nombre[]) {
    int i;

    for (i = 0; i < cantidad; i++) {
        if (nombresIguales(nombres[i], nombre)) {
            return i;
        }
    }

    return -1;
}

void registrar(char nombres[][MAX_NOMBRE], int demanda[],
               int minutos[], int recursos[], int *cantidad) {
    char nombre[MAX_NOMBRE];

    if (*cantidad >= MAX_PRODUCTOS) {
        printf("Error: limite de cinco productos alcanzado.\n");
        return;
    }

    printf("\n--- REGISTRAR PRODUCTO ---\n");

    leerNombre(nombre);

    if (buscarIndice(nombres, *cantidad, nombre) != -1) {
        printf("Error: el producto ya existe.\n");
        return;
    }

    strcpy(nombres[*cantidad], nombre);

    demanda[*cantidad] = (int)leerEntero(
        "Demanda en unidades: ", 0, MAX_DEMANDA);

    minutos[*cantidad] = (int)leerEntero(
        "Minutos de fabricacion por unidad: ", 1, MAX_POR_UNIDAD);

    recursos[*cantidad] = (int)leerEntero(
        "Recursos necesarios por unidad: ", 1, MAX_POR_UNIDAD);

    (*cantidad)++;

    printf("Producto registrado correctamente.\n");
}

void listar(char nombres[][MAX_NOMBRE], int demanda[],
            int minutos[], int recursos[], int cantidad) {
    int i;

    if (cantidad == 0) {
        printf("No hay productos registrados.\n");
        return;
    }

    printf("\n--- PRODUCTOS REGISTRADOS ---\n");

    printf("%-4s %-39s %8s %8s %10s\n",
           "No.", "Nombre", "Demanda", "Min/u", "Recurso/u");

    for (i = 0; i < cantidad; i++) {
        printf("%-4d %-39s %8d %8d %10d\n",
               i + 1, nombres[i], demanda[i],
               minutos[i], recursos[i]);
    }

    printf("\nProductos registrados: %d de %d\n",
           cantidad, MAX_PRODUCTOS);
}

int solicitarIndice(char nombres[][MAX_NOMBRE], int cantidad) {
    char nombre[MAX_NOMBRE];
    int posicion;

    if (cantidad == 0) {
        printf("No hay productos registrados.\n");
        return -1;
    }

    leerNombre(nombre);

    posicion = buscarIndice(nombres, cantidad, nombre);

    if (posicion == -1) {
        printf("Error: producto no encontrado.\n");
    }

    return posicion;
}

void buscarProducto(char nombres[][MAX_NOMBRE], int demanda[],
                    int minutos[], int recursos[], int cantidad) {
    int i;

    printf("\n--- BUSCAR PRODUCTO ---\n");

    i = solicitarIndice(nombres, cantidad);

    if (i == -1) {
        return;
    }

    printf("\nProducto encontrado:\n");
    printf("Nombre: %s\n", nombres[i]);
    printf("Demanda: %d unidades\n", demanda[i]);
    printf("Minutos por unidad: %d\n", minutos[i]);
    printf("Recursos por unidad: %d\n", recursos[i]);
}

void editar(char nombres[][MAX_NOMBRE], int demanda[],
            int minutos[], int recursos[], int cantidad) {
    int i;
    int existe;
    char nuevo[MAX_NOMBRE];

    printf("\n--- EDITAR PRODUCTO ---\n");

    i = solicitarIndice(nombres, cantidad);

    if (i == -1) {
        return;
    }

    printf("Producto seleccionado: %s\n", nombres[i]);
    printf("Ingrese los nuevos datos:\n");

    while (1) {
        leerNombre(nuevo);

        existe = buscarIndice(nombres, cantidad, nuevo);

        if (existe == -1 || existe == i) {
            break;
        }

        printf("Error: otro producto tiene ese nombre.\n");
    }

    strcpy(nombres[i], nuevo);

    demanda[i] = (int)leerEntero(
        "Nueva demanda: ", 0, MAX_DEMANDA);

    minutos[i] = (int)leerEntero(
        "Nuevos minutos por unidad: ", 1, MAX_POR_UNIDAD);

    recursos[i] = (int)leerEntero(
        "Nuevos recursos por unidad: ", 1, MAX_POR_UNIDAD);

    printf("Producto actualizado correctamente.\n");
}

void eliminar(char nombres[][MAX_NOMBRE], int demanda[],
              int minutos[], int recursos[], int *cantidad) {
    int i;
    int j;
    int confirmar;

    printf("\n--- ELIMINAR PRODUCTO ---\n");

    i = solicitarIndice(nombres, *cantidad);

    if (i == -1) {
        return;
    }

    printf("Producto seleccionado: %s\n", nombres[i]);

    confirmar = (int)leerEntero(
        "Confirmar eliminacion (1 = Si, 0 = No): ", 0, 1);

    if (confirmar == 0) {
        printf("Eliminacion cancelada.\n");
        return;
    }

    for (j = i; j < *cantidad - 1; j++) {
        strcpy(nombres[j], nombres[j + 1]);
        demanda[j] = demanda[j + 1];
        minutos[j] = minutos[j + 1];
        recursos[j] = recursos[j + 1];
    }

    (*cantidad)--;

    nombres[*cantidad][0] = '\0';
    demanda[*cantidad] = 0;
    minutos[*cantidad] = 0;
    recursos[*cantidad] = 0;

    printf("Producto eliminado correctamente.\n");
}

void configurar(long long *tiempoDisponible,
                long long *stockDisponible,
                int *limitesConfigurados) {
    printf("\n--- LIMITES DE PRODUCCION ---\n");

    *tiempoDisponible = leerEntero(
        "Tiempo disponible en minutos: ", 0, MAX_CAPACIDAD);

    *stockDisponible = leerEntero(
        "Recursos disponibles: ", 0, MAX_CAPACIDAD);

    *limitesConfigurados = 1;

    printf("Limites configurados correctamente.\n");
}

void evaluar(char nombres[][MAX_NOMBRE], int demanda[],
             int minutos[], int recursos[], int cantidad,
             long long tiempoDisponible, long long stockDisponible,
             int limitesConfigurados) {
    int i;
    long long tiempoTotal = 0;
    long long recursosTotales = 0;
    long long tiempoProducto;
    long long recursosProducto;

    if (cantidad == 0) {
        printf("Registre productos antes de evaluar.\n");
        return;
    }

    if (limitesConfigurados == 0) {
        printf("Configure primero los limites en la opcion 6.\n");
        return;
    }

    printf("\n--- REPORTE DE PRODUCCION ---\n");

    printf("%-39s %18s %18s\n",
           "Producto", "Tiempo (min)", "Recursos");

    for (i = 0; i < cantidad; i++) {
        tiempoProducto = (long long)demanda[i] * minutos[i];
        recursosProducto = (long long)demanda[i] * recursos[i];

        tiempoTotal += tiempoProducto;
        recursosTotales += recursosProducto;

        printf("%-39s %18lld %18lld\n",
               nombres[i], tiempoProducto, recursosProducto);
    }

    printf("\n--- RESUMEN DE PRODUCCION ---\n");

    printf("Tiempo requerido: %lld minutos\n", tiempoTotal);
    printf("Recursos requeridos: %lld unidades\n", recursosTotales);
    printf("Tiempo disponible: %lld minutos\n", tiempoDisponible);
    printf("Recursos disponibles: %lld unidades\n", stockDisponible);

    printf("\n--- EVALUACION DE FACTIBILIDAD ---\n");

    if (tiempoTotal <= tiempoDisponible &&
        recursosTotales <= stockDisponible) {
        printf("SI se puede cumplir la demanda.\n");
    } else {
        printf("NO se puede cumplir la demanda.\n");
    }

    if (tiempoTotal > tiempoDisponible) {
        printf("Deficit de tiempo: %lld minutos\n",
               tiempoTotal - tiempoDisponible);
    } else {
        printf("Tiempo sobrante: %lld minutos\n",
               tiempoDisponible - tiempoTotal);
    }

    if (recursosTotales > stockDisponible) {
        printf("Deficit de recursos: %lld unidades\n",
               recursosTotales - stockDisponible);
    } else {
        printf("Recursos sobrantes: %lld unidades\n",
               stockDisponible - recursosTotales);
    }
}

int main(void) {
    char nombres[MAX_PRODUCTOS][MAX_NOMBRE] = {{0}};
    int demanda[MAX_PRODUCTOS] = {0};
    int minutos[MAX_PRODUCTOS] = {0};
    int recursos[MAX_PRODUCTOS] = {0};
    int cantidad = 0;
    int opcion;
    int limitesConfigurados = 0;

    long long tiempoDisponible = 0;
    long long stockDisponible = 0;

    printf("========================================\n");
    printf("  SISTEMA DE CONTROL DE PRODUCCION RC1\n");
    printf("========================================\n");

    do {
        printf("\n--- MENU PRINCIPAL ---\n");
        printf("1. Registrar producto\n");
        printf("2. Listar productos\n");
        printf("3. Buscar producto\n");
        printf("4. Editar producto\n");
        printf("5. Eliminar producto\n");
        printf("6. Configurar limites de produccion\n");
        printf("7. Calcular necesidades y factibilidad\n");
        printf("0. Salir\n");

        printf("\nProductos registrados: %d/%d\n",
               cantidad, MAX_PRODUCTOS);

        if (limitesConfigurados == 1) {
            printf("Limites de produccion: Configurados\n");
        } else {
            printf("Limites de produccion: Pendientes\n");
        }

        opcion = (int)leerEntero(
            "\nSeleccione una opcion: ", 0, 7);

        switch (opcion) {
            case 1:
                registrar(nombres, demanda, minutos,
                          recursos, &cantidad);
                break;

            case 2:
                listar(nombres, demanda, minutos,
                       recursos, cantidad);
                break;

            case 3:
                buscarProducto(nombres, demanda, minutos,
                               recursos, cantidad);
                break;

            case 4:
                editar(nombres, demanda, minutos,
                       recursos, cantidad);
                break;

            case 5:
                eliminar(nombres, demanda, minutos,
                         recursos, &cantidad);
                break;

            case 6:
                configurar(&tiempoDisponible, &stockDisponible,
                           &limitesConfigurados);
                break;

            case 7:
                evaluar(nombres, demanda, minutos, recursos,
                        cantidad, tiempoDisponible,
                        stockDisponible, limitesConfigurados);
                break;

            case 0:
                printf("\nPrograma finalizado correctamente.\n");
                break;
        }

    } while (opcion != 0);

    return 0;
}
