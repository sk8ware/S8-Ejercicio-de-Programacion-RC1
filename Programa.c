#include <stdio.h>
#include <string.h>

#define MAX_PROD 5
#define MAX_INS 5

char prodNom[MAX_PROD][20];
float prodTiempo[MAX_PROD];
float prodRecursos[MAX_PROD];
int prodDem[MAX_PROD];
int numProd = 0;
int opcion;

char insNom[MAX_INS][50];
float insCant[MAX_INS];
int numIns = 0;
float consumo[MAX_PROD][MAX_INS];

void registrarInsumos(char insNom[][50], float insCant[], int *numIns);
void reponerInsumo(char insNom[][50], float insCant[], int numIns);
float calcularTiempoTotal(int prodDem[], float prodTiempo[], int numProd);
void calcularRecursos(float recurso[], int prodDem[], int numProd, float consumo[][MAX_INS], int numIns, float req[]);
void verificarDemanda(float prodTiempo[], float tiempoDisp, int numProd, int prodDem[], float consumo[][MAX_INS], char insNom[][50], int numIns, float insCant[]);
void registrarProducto(char prodNom[][20], float prodTiempo[], float prodRecursos[], int prodDem[], int *numProd, char insNom[][50], int numIns, float consumo[][MAX_INS]);
void editarProducto(char prodNom[][20], float prodTiempo[], float consumo[][MAX_INS], int prodDem[], int numProd, char insNom[][50], int numIns);
void eliminarProducto(char prodNom[][20], float prodTiempo[], float consumo[][MAX_INS], int prodDem[], int *numProd, char insNom[][50], int numIns);
void mostrarProductos(char prodNom[][20], float prodTiempo[], float consumo[][MAX_INS], int prodDem[], int numProd, char insNom[][50], int numIns);

void registrarInsumos(char insNom[][50], float insCant[], int *numIns) {
    int ok;
    do {
        printf("¿Cuantos tipos de insumo desea registrar? (maximo %d)\n", MAX_INS);
        ok = scanf("%d", numIns);
        while (getchar() != '\n');
        if (ok != 1 || *numIns < 0 || *numIns > MAX_INS) {
            printf("Error: el maximo permitido es %d\n", MAX_INS);
        }
    } while (ok != 1 || *numIns < 0 || *numIns > MAX_INS);

    for (int i = 0; i < *numIns; i++) {
        printf("Escriba el nombre del insumo numero %d\n", i + 1);
        scanf("%49s", insNom[i]);
        while (getchar() != '\n');
        do {
            printf("Escriba la cantidad disponible de este insumo\n");
            ok = scanf("%f", &insCant[i]);
            while (getchar() != '\n');
            if (ok != 1 || insCant[i] < 0) {
                printf("Error: la cantidad no puede ser negativa\n");
            }
        } while (ok != 1 || insCant[i] < 0);
    }

    printf("\n\n#\t\tMaterial\t\tDisponible\n");
    for (int i = 0; i < *numIns; i++) {
        printf("%d\t\t%-19s\t%.2f\n", i + 1, insNom[i], insCant[i]);
    }
}

void reponerInsumo(char insNom[][50], float insCant[], int numIns) {
    char clave[50];
    float extra = 0;
    int pos = -1;
    int ok;

    if (numIns == 0) {
        printf("Aun no hay insumos registrados\n");
        return;
    }

    printf("======== STOCK ACTUAL ========\n");
    printf("\n #\t\tInsumo\t\tExistencia\n");
    for (int j = 0; j < numIns; j++) {
        printf("%d\t\t%s\t\t%.2f\n", j + 1, insNom[j], insCant[j]);
    }

    printf("Que insumo desea reabastecer?\n");
    scanf("%49s", clave);
    while (getchar() != '\n');

    for (int i = 0; i < numIns; i++) {
        if (strcmp(clave, insNom[i]) == 0) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        printf("Ese insumo no esta registrado\n");
        return;
    }

    do {
        printf("Cantidad a sumar a %s: ", insNom[pos]);
        ok = scanf("%f", &extra);
        while (getchar() != '\n');
        if (ok != 1 || extra <= 0) {
            printf("Error: el valor debe ser mayor que cero\n");
        }
    } while (ok != 1 || extra <= 0);

    insCant[pos] += extra;
    printf("Nuevo stock de %s: %.2f unidades.\n", insNom[pos], insCant[pos]);
}

void registrarProducto(char prodNom[][20], float prodTiempo[], float prodRecursos[], int prodDem[], int *numProd, char insNom[][50], int numIns, float consumo[][MAX_INS]) {
    if (*numProd >= MAX_PROD) {
        printf("\nNo se puede agregar mas: ya hay %d productos (maximo).\n", MAX_PROD);
        return;
    }

    int i = *numProd;
    int ok;

    printf("\n*** NUEVO PRODUCTO (%d de %d) ***\n", i + 1, MAX_PROD);
    printf("Nombre del producto (sin espacios): ");
    scanf("%19s", prodNom[i]);

    do {
        printf("Unidades demandadas: ");
        ok = scanf("%d", &prodDem[i]);
        while (getchar() != '\n');
        if (ok != 1 || prodDem[i] < 0) {
            printf("Error: escriba un numero entero valido\n");
        }
    } while (ok != 1 || prodDem[i] < 0);

    do {
        printf("Horas necesarias para fabricar una unidad: ");
        ok = scanf("%f", &prodTiempo[i]);
        while (getchar() != '\n');
        if (ok != 1 || prodTiempo[i] < 0) {
            printf("Error: escriba un numero valido\n");
        }
    } while (ok != 1 || prodTiempo[i] < 0);

    for (int j = 0; j < numIns; j++) {
        do {
            printf("Consumo de %s por cada unidad: ", insNom[j]);
            ok = scanf("%f", &consumo[i][j]);
            while (getchar() != '\n');
            if (ok != 1 || consumo[i][j] < 0) {
                printf("No se permiten numeros negativos\n");
            }
        } while (ok != 1 || consumo[i][j] < 0);
    }

    (*numProd)++;
}

float calcularTiempoTotal(int prodDem[], float prodTiempo[], int numProd) {
    float acumulado = 0.0;
    for (int i = 0; i < numProd; i++) {
        float parcial = prodDem[i] * prodTiempo[i];
        acumulado += parcial;
    }
    return acumulado;
}

void calcularRecursos(float recurso[], int prodDem[], int numProd, float consumo[][MAX_INS], int numIns, float req[]) {
    for (int j = 0; j < numIns; j++) {
        req[j] = 0.0;
    }
    for (int i = 0; i < numProd; i++) {
        for (int j = 0; j < numIns; j++) {
            req[j] += consumo[i][j] * prodDem[i];
        }
    }
}

void verificarDemanda(float prodTiempo[], float tiempoDisp, int numProd, int prodDem[], float consumo[][MAX_INS], char insNom[][50], int numIns, float insCant[]) {
    float tiempoReq = calcularTiempoTotal(prodDem, prodTiempo, numProd);
    float req[MAX_INS];
    calcularRecursos(prodRecursos, prodDem, numProd, consumo, numIns, req);
    int insumosOk = 1;

    printf("Horas necesarias: %.2f | Horas disponibles: %.2f\n", tiempoReq, tiempoDisp);
    if (tiempoReq > tiempoDisp) {
        printf("Faltan %.2f horas de produccion\n", tiempoReq - tiempoDisp);
    }

    printf("Revision de insumos:\n");
    for (int j = 0; j < numIns; j++) {
        printf("%s -> necesita: %.2f | hay: %.2f\n", insNom[j], req[j], insCant[j]);
        if (req[j] > insCant[j]) {
            insumosOk = 0;
            printf("   Faltan %.2f unidades de %s\n", req[j] - insCant[j], insNom[j]);
        }
    }

    if (tiempoReq <= tiempoDisp && insumosOk) {
        printf("La fabrica SI puede cumplir la demanda\n");
    } else {
        printf("La fabrica NO puede cumplir la demanda\n");
    }
}

void mostrarProductos(char prodNom[][20], float prodTiempo[], float consumo[][MAX_INS], int prodDem[], int numProd, char insNom[][50], int numIns) {
    if (numProd == 0) {
        printf("\n[!] Todavia no hay productos cargados.\n");
        return;
    }

    printf("\n#\tNombre\t\tHoras/Unid\tPedido\t\tInsumos por unidad\n");
    for (int i = 0; i < numProd; i++) {
        printf("%d\t%-12s\t%.2f\t\t%d\t\t", i + 1, prodNom[i], prodTiempo[i], prodDem[i]);
        for (int j = 0; j < numIns; j++) {
            printf("%s: %.2f ", insNom[j], consumo[i][j]);
        }
        printf("\n");
    }
}

void editarProducto(char prodNom[][20], float prodTiempo[], float consumo[][MAX_INS], int prodDem[], int numProd, char insNom[][50], int numIns) {
    if (numProd == 0) {
        printf("\n[!] No existen productos para editar.\n");
        return;
    }

    mostrarProductos(prodNom, prodTiempo, consumo, prodDem, numProd, insNom, numIns);

    char clave[20];
    int pos = -1;
    int ok;

    printf("\nEscriba el nombre exacto del producto que quiere editar: ");
    scanf("%19s", clave);
    while (getchar() != '\n');

    for (int i = 0; i < numProd; i++) {
        if (strcmp(clave, prodNom[i]) == 0) {
            pos = i;
            break;
        }
    }

    if (pos != -1) {
        printf("\nProducto localizado. Escriba los datos nuevos:\n");

        printf("Nombre nuevo: ");
        scanf("%19s", prodNom[pos]);

        do {
            printf("Horas nuevas por unidad: ");
            ok = scanf("%f", &prodTiempo[pos]);
            while (getchar() != '\n');
            if (ok != 1 || prodTiempo[pos] < 0) {
                printf("Error: escriba un numero valido\n");
            }
        } while (ok != 1 || prodTiempo[pos] < 0);

        do {
            printf("Demanda nueva: ");
            ok = scanf("%d", &prodDem[pos]);
            while (getchar() != '\n');
            if (ok != 1 || prodDem[pos] < 0) {
                printf("Error: escriba un numero entero valido\n");
            }
        } while (ok != 1 || prodDem[pos] < 0);

        for (int j = 0; j < numIns; j++) {
            do {
                printf("Nuevo consumo de %s por unidad: ", insNom[j]);
                ok = scanf("%f", &consumo[pos][j]);
                while (getchar() != '\n');
                if (ok != 1 || consumo[pos][j] < 0) {
                    printf("El valor no puede ser negativo (cero es valido)\n");
                }
            } while (ok != 1 || consumo[pos][j] < 0);
        }

        printf("\nProducto actualizado con exito.\n");
    } else {
        printf("\nNo se encontro ese producto.\n");
    }
}

void eliminarProducto(char prodNom[][20], float prodTiempo[], float consumo[][MAX_INS], int prodDem[], int *numProd, char insNom[][50], int numIns) {
    if (*numProd == 0) {
        printf("\n[!] No existen productos para eliminar.\n");
        return;
    }

    mostrarProductos(prodNom, prodTiempo, consumo, prodDem, *numProd, insNom, numIns);

    char clave[20];
    int pos = -1;

    printf("\nEscriba el nombre del producto a borrar: ");
    scanf("%19s", clave);
    while (getchar() != '\n');

    for (int i = 0; i < *numProd; i++) {
        if (strcmp(clave, prodNom[i]) == 0) {
            pos = i;
            break;
        }
    }

    if (pos != -1) {
        for (int i = pos; i < *numProd - 1; i++) {
            strcpy(prodNom[i], prodNom[i + 1]);
            prodTiempo[i] = prodTiempo[i + 1];
            prodDem[i] = prodDem[i + 1];
            for (int j = 0; j < numIns; j++) {
                consumo[i][j] = consumo[i + 1][j];
            }
        }
        (*numProd)--;
        printf("\nProducto borrado con exito.\n");
    } else {
        printf("\nNo se encontro ese producto.\n");
    }
}

int main(int argc, char const *argv[]) {
    do {
        printf("\n****************************************\n");
        printf("     CONTROL DE PRODUCCION - PLANTA     \n");
        printf("****************************************\n");
        printf("1. Cargar inventario de insumos\n");
        printf("2. Reponer stock de un insumo\n");
        printf("3. Agregar un producto\n");
        printf("4. Ver lista de productos\n");
        printf("5. Obtener horas totales de fabricacion\n");
        printf("6. Obtener insumos totales necesarios\n");
        printf("7. Comprobar si se puede cumplir el pedido\n");
        printf("8. Modificar un producto\n");
        printf("9. Quitar un producto\n");
        printf("10. Terminar programa\n");
        printf("........................................\n");
        printf("Elija una opcion (1-10): ");

        if (scanf("%d", &opcion) != 1) {
            printf("Error: debe escribir un numero.\n");
            while (getchar() != '\n');
            opcion = 0;
            continue;
        }
        while (getchar() != '\n');

        switch (opcion) {
            case 1:
                if (numProd > 0) {
                    printf("\nPrimero elimine los productos para volver a cargar el inventario.\n");
                } else {
                    registrarInsumos(insNom, insCant, &numIns);
                }
                break;

            case 2:
                reponerInsumo(insNom, insCant, numIns);
                break;

            case 3:
                if (numIns == 0) {
                    printf("\nAntes debe cargar el inventario de insumos.\n");
                } else {
                    registrarProducto(prodNom, prodTiempo, prodRecursos, prodDem, &numProd, insNom, numIns, consumo);
                }
                break;

            case 4:
                mostrarProductos(prodNom, prodTiempo, consumo, prodDem, numProd, insNom, numIns);
                break;

            case 5:
                if (numProd == 0) {
                    printf("\nAun no se han cargado productos.\n");
                } else {
                    float totalHoras = calcularTiempoTotal(prodDem, prodTiempo, numProd);
                    printf("\n>> Horas totales de fabricacion: %.2f\n", totalHoras);
                }
                break;

            case 6:
                if (numProd == 0) {
                    printf("\nAun no se han cargado productos.\n");
                } else {
                    float req[MAX_INS];
                    calcularRecursos(prodRecursos, prodDem, numProd, consumo, numIns, req);
                    printf("\nInsumos necesarios para cubrir el pedido:\n");
                    for (int j = 0; j < numIns; j++) {
                        printf("# %s: necesita %.2f | en stock %.2f\n", insNom[j], req[j], insCant[j]);
                    }
                }
                break;

            case 7:
                if (numProd == 0) {
                    printf("\n[!] Aun no se han cargado productos.\n");
                } else {
                    float tiempoDisp;
                    int ok;
                    do {
                        printf("\nHoras disponibles de la fabrica: ");
                        ok = scanf("%f", &tiempoDisp);
                        while (getchar() != '\n');
                        if (ok != 1 || tiempoDisp < 0) {
                            printf("Error: escriba un numero valido\n");
                        }
                    } while (ok != 1 || tiempoDisp < 0);
                    verificarDemanda(prodTiempo, tiempoDisp, numProd, prodDem, consumo, insNom, numIns, insCant);
                }
                break;

            case 8:
                editarProducto(prodNom, prodTiempo, consumo, prodDem, numProd, insNom, numIns);
                break;

            case 9:
                eliminarProducto(prodNom, prodTiempo, consumo, prodDem, &numProd, insNom, numIns);
                break;

            case 10:
                printf("\nPrograma finalizado. ¡Hasta pronto!\n");
                return 0;

            default:
                printf("\n[X] Opcion no valida, vuelva a intentar.\n");
                break;
        }

        if (opcion != 10) {
            int continuar = 0;
            printf("\n¿Quiere hacer otra gestion? (Si: 1 / No: 0): ");
            scanf("%d", &continuar);
            while (getchar() != '\n');
            if (continuar != 1) {
                printf("\nPrograma finalizado. ¡Hasta pronto!\n");
                break;
            }
        }
    } while (1);

    return 0;
}
