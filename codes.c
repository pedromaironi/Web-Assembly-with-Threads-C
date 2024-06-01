#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <emscripten.h>

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
    SortTask* task = (SortTask*)arg;
    int* array = task->array;
    int n = task->n;
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (array[j] > array[j+1]) {
                int temp = array[j];
                array[j] = array[j+1];
                array[j+1] = temp;
            }
        }
    }
    printf("Array ordenado por burbuja: ");
    print_array(array, n);
    return NULL;
}

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
    SortTask* task = (SortTask*)arg;
    int* array = task->array;
    int n = task->n;
    quicksort(array, 0, n - 1);
    printf("arrayay ordenado por quicksort: ");
    print_array(array, n);
    return NULL;
}

void* insercion_thread(void* arg) {
    SortTask* task = (SortTask*)arg;
    int* array = task->array;
    int n = task->n;
    for (int i = 1; i < n; i++) {
        int key = array[i];
        int j = i - 1;
        while (j >= 0 && array[j] > key) {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = key;
    }
    printf("Array ordenado por inserción: ");
    print_array(array, n);
    return NULL;
}

void* busqueda_secuencial_thread(void* arg) {
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
        printf("Elemento %d encontrado en la posición %d usando búsqueda secuencial.\n", key, found);
    } else {
        printf("Elemento %d no encontrado usando búsqueda secuencial.\n", key);
    }
    return NULL;
}

void* busqueda_binaria_thread(void* arg) {
    SearchTask* task = (SearchTask*)arg;
    int* array = task->array;
    int n = task->n;
    int key = task->key;
    int low = 0;
    int high = n - 1;
    int found = -1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (array[mid] == key) {
            found = mid;
            break;
        } else if (array[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    if (found != -1) {
        printf("Elemento %d encontrado en la posición %d usando búsqueda binaria.\n", key, found);
    } else {
        printf("Elemento %d no encontrado usando búsqueda binaria.\n", key);
    }
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

    SortTask task1 = {array1, n1};
    SortTask task2 = {array2, n2};
    SortTask task3 = {array3, n3};

    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, burbuja_thread, (void*)&task1);
    pthread_create(&t2, NULL, quicksort_thread, (void*)&task2);
    pthread_create(&t3, NULL, insercion_thread, (void*)&task3);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    int key1 = 1;
    int key2 = 4;

    SearchTask searchTask1 = {array4, n4, key1};
    SearchTask searchTask2 = {array5, n5, key2};

    pthread_t t4, t5;

    pthread_create(&t4, NULL, busqueda_secuencial_thread, (void*)&searchTask1);
    pthread_create(&t5, NULL, busqueda_binaria_thread, (void*)&searchTask2);

    pthread_join(t4, NULL);
    pthread_join(t5, NULL);

    return 0;
}