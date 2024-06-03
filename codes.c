#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <emscripten.h>


// Estructuras para tareas de ordenacioon y busqueda
typedef struct {
    int *array;
    int n;
} SortTask;

typedef struct {
    int *array;
    int n;
    int key;
} SearchTask;

void print_array(int array[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void* burbuja_thread(void* arg) {
    // Convertir el argumento al tipo adecuado
    SortTask* task = (SortTask*)arg;
    int* array = task->array;
    int n = task->n;
    // Algoritmo de ordenacion por burbuja
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (array[j] > array[j+1]) {
                int temp = array[j];
                array[j] = array[j+1];
                array[j+1] = temp;
            }
        }
    }
    printf("Arreglo ordenado por burbuja: ");
    print_array(array, n);
    return NULL;
}

// Funcion auxiliar para la particion en QuickSort
int partition(int array[], int low, int high) {
    int pivot = array[high];
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (array[j] < pivot) {
            i++;
            int temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }
    int temp = array[i + 1];
    array[i + 1] = array[high];
    array[high] = temp;
    return (i + 1);
}

void quicksort(int array[], int low, int high) {
    if (low < high) {
        int pi = partition(array, low, high);
        quicksort(array, low, pi - 1);
        quicksort(array, pi + 1, high);
    }
}

void* quicksort_thread(void* arg) {
    // Convertir el argumento al tipo adecuado - void
    SortTask* task = (SortTask*)arg;
    int* array = task->array;
    int n = task->n;
    
    // Llamada a QuickSort para ordenar el array
    quicksort(array, 0, n - 1);

    // Imprimir el array 
    printf("Arreglo ordenado por quicksort: ");
    print_array(array, n);
    return NULL;
}

void* insercion_thread(void* arg) {
    // Convertir el argumento al tipo adecuado
    SortTask* task = (SortTask*)arg;
    
    // Obtener el array y su tamaño
    int* array = task->array;
    int n = task->n;
    
    // Algoritmo de ordenacion por insercion
    for (int i = 1; i < n; i++) {
        // Seleccionar el elemento actual como la clave
        int key = array[i];
        
        // Inicializar el índice para comparacion
        int j = i - 1;
        
        // Mover los elementos mayores que la clave a la derecha
        // mientras se asegura que el array está ordenado hasta el indice i
        while (j >= 0 && array[j] > key) {
            array[j + 1] = array[j];
            j = j - 1;
        }
        
        // Insertar la clave en su posicion correcta en el array ordenado
        array[j + 1] = key;
    }
    
    printf("Arreglo ordenado por insercion: ");
    print_array(array, n);
    
    return NULL;
}

void* busqueda_secuencial_thread(void* arg) {
    // Conversion de datos - void - array con puntero
    SearchTask* task = (SearchTask*)arg;
    int* array = task->array;
    int n = task->n;
    int key = task->key;
    int found = -1;

    for (int i = 0; i < n; i++) {
        if (array[i] == key) {
            found = i;
            break;
        }
    }

    if (found != -1) {
        printf("Elemento %d encontrado en la posicion %d usando busqueda secuencial\n", key, found);
    } else {
        printf("Elemento %d no encontrado usando busqueda secuencial\n", key);
    }
    return NULL;
}

void* busqueda_binaria_thread(void* arg) {
    // Convertir el argumento al tipo adecuado (SearchTask)
    SearchTask* task = (SearchTask*)arg;
    
    // Obtener el array y su tamaño, así como la clave de búsqueda
    int* array = task->array;
    int n = task->n;
    int key = task->key;
    
    // Inicializar los índices bajo (low) y alto (high) del array
    int low = 0;
    int high = n - 1;
    
    // Inicializar una variable para almacenar la posición del elemento encontrado
    int encontrado = -1;

    // Bucle principal del algoritmo de busqueda binaria
    while (low <= high) {
        // Calcular el índice medio del array
        int medio = low + (high - low) / 2;

        // Comprobar si el elemento en el índice medio es igual a la clave de busqueda
        if (array[medio] == key) {
            // Si se encuentra el elemento, almacenar su posición y salir del bucle
            encontrado = medio;
            break;
        } else if (array[medio] < key) {
            // Si el elemento en el índice medio es menor que la clave de búsqueda,
            // ajustar el límite inferior para buscar en la mitad derecha del array
            low = medio + 1;
        } else {
            // Si el elemento en el índice medio es mayor que la clave de búsqueda,
            // ajustar el límite superior para buscar en la mitad izquierda del array
            high = medio - 1;
        }
    }

    // Después de salir del bucle, verificar si se encontró el elemento
    if (encontrado != -1) {
        // Si se encontró, imprimir su posición
        printf("Elemento %d encontrado en la posicion %d usando búsqueda binaria.\n", key, encontrado);
    } else {
        // Si no se encontró, imprimir un mensaje indicando que el elemento no fue encontrado
        printf("Elemento %d no encontrado usando búsqueda binaria.\n", key);
    }

    // Devolver NULL ya que se requiere un valor de retorno para la función en un hilo
    return NULL;
}

int main() {
    // Datos para los algoritmos de ordenación
    int array1[] = {2, 7, 25, 34, 1, 32};
    int array2[] = {45, 7, 23, 65, 5, 62};
    int array3[] = {65, 43, 23, 22, 2, 1};
    int array4[] = {1, 3, 5, 7, 9, 11};
    int array5[] = {2, 4, 6, 8, 10, 12};

    int n1 = sizeof(array1)/sizeof(array1[0]);
    int n2 = sizeof(array2)/sizeof(array2[0]);
    int n3 = sizeof(array3)/sizeof(array3[0]);
    int n4 = sizeof(array4)/sizeof(array4[0]);
    int n5 = sizeof(array5)/sizeof(array5[0]);

    // Creación de tareas de ordenación

    SortTask task1 = {array1, n1};
    SortTask task2 = {array2, n2};
    SortTask task3 = {array3, n3};
    
    // Creacion de hilos para ejecutar los algoritmos de ordenacion
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, burbuja_thread, (void*)&task1);
    pthread_create(&t2, NULL, quicksort_thread, (void*)&task2);
    pthread_create(&t3, NULL, insercion_thread, (void*)&task3);

    // Esperar a que los hilos de ordenacion terminen
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    // Declaracion y asignacion de datos para los algoritmos de busqueda
    int key1 = 1;
    int key2 = 4;

    // Creacion de tareas de busqueda
    SearchTask searchTask1 = {array4, n4, key1};
    SearchTask searchTask2 = {array5, n5, key2};

    // Creacion de hilos para ejecutar los algoritmos de busqueda   
    pthread_t t4, t5;

    pthread_create(&t4, NULL, busqueda_secuencial_thread, (void*)&searchTask1);
    pthread_create(&t5, NULL, busqueda_binaria_thread, (void*)&searchTask2);

    // Esperar a que los hilos de búsqueda terminen
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);

    return 0;
}