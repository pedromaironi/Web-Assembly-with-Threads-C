#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <emscripten.h>

typedef struct {
    int *arr;
    int n;
} SortTask;

typedef struct {
    int *arr;
    int n;
    int key;
} SearchTask;

void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void* burbuja_thread(void* arg) {
    SortTask* task = (SortTask*)arg;
    int* arr = task->arr;
    int n = task->n;
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
    printf("Array ordenado por burbuja: ");
    print_array(arr, n);
    return NULL;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return (i + 1);
}

void* quicksort_thread(void* arg) {
    SortTask* task = (SortTask*)arg;
    int* arr = task->arr;
    int low = 0;
    int high = task->n - 1;
    if (low < high) {
        int pi = partition(arr, low, high);
        SortTask leftTask = {arr, pi};
        SortTask rightTask = {arr + pi + 1, high - pi};
        quicksort_thread((void*)&leftTask);
        quicksort_thread((void*)&rightTask);
    }
    printf("Array ordenado por quicksort: ");
    print_array(arr, task->n);
    return NULL;
}

void* insercion_thread(void* arg) {
    SortTask* task = (SortTask*)arg;
    int* arr = task->arr;
    int n = task->n;
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
    printf("Array ordenado por inserción: ");
    print_array(arr, n);
    return NULL;
}

void* busqueda_secuencial_thread(void* arg) {
    SearchTask* task = (SearchTask*)arg;
    int* arr = task->arr;
    int n = task->n;
    int key = task->key;
    int found = -1;

    for (int i = 0; i < n; i++) {
        if (arr[i] == key) {
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
    int* arr = task->arr;
    int n = task->n;
    int key = task->key;
    int low = 0;
    int high = n - 1;
    int found = -1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (arr[mid] == key) {
            found = mid;
            break;
        } else if (arr[mid] < key) {
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
    int arr1[] = {2, 7, 25, 34, 1, 32};
    int arr2[] = {45, 7, 23, 65, 5, 62};
    int arr3[] = {65, 43, 23, 22, 2, 1};
    int arr4[] = {1, 3, 5, 7, 9, 11};
    int arr5[] = {2, 4, 6, 8, 10, 12};

    int n1 = sizeof(arr1)/sizeof(arr1[0]);
    int n2 = sizeof(arr2)/sizeof(arr2[0]);
    int n3 = sizeof(arr3)/sizeof(arr3[0]);
    int n4 = sizeof(arr4)/sizeof(arr4[0]);
    int n5 = sizeof(arr5)/sizeof(arr5[0]);

    SortTask task1 = {arr1, n1};
    SortTask task2 = {arr2, n2};
    SortTask task3 = {arr3, n3};

    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, burbuja_thread, (void*)&task1);
    pthread_create(&t2, NULL, quicksort_thread, (void*)&task2);
    pthread_create(&t3, NULL, insercion_thread, (void*)&task3);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    int key1 = 1;
    int key2 = 4;

    SearchTask searchTask1 = {arr4, n4, key1};
    SearchTask searchTask2 = {arr5, n5, key2};

    pthread_t t4, t5;

    pthread_create(&t4, NULL, busqueda_secuencial_thread, (void*)&searchTask1);
    pthread_create(&t5, NULL, busqueda_binaria_thread, (void*)&searchTask2);

    pthread_join(t4, NULL);
    pthread_join(t5, NULL);

    return 0;
}