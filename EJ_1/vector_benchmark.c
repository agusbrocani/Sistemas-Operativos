#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define N 10000000
#define N_THREADS 8
#define RANGO (N / N_THREADS)

double vector[N];

typedef struct {
    int inicio;
    int fin;
} hilo_args;

void* hilo_func(void* arg) {
    hilo_args* args = (hilo_args*)arg;
    for (int i = args->inicio; i < args->fin; i++) {
        vector[i] = vector[i] * 2.0;
    }
    pthread_exit(NULL);
}

long millis() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

void inicializar_vector() {
    for (int i = 0; i < N; i++) vector[i] = 1.0;
}

int main() {
    pthread_t threads[N_THREADS];
    hilo_args args[N_THREADS];

    inicializar_vector();
    long t1 = millis();
    for (int i = 0; i < N; i++) {
        vector[i] = vector[i] * 2.0;
    }
    long t2 = millis();
    printf("Tiempo secuencial: %ld ms\n", t2 - t1);

    inicializar_vector();
    long t3 = millis();
    for (int i = 0; i < N_THREADS; i++) {
        args[i].inicio = i * RANGO;
        args[i].fin = args[i].inicio + RANGO;
        pthread_create(&threads[i], NULL, hilo_func, &args[i]);
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    long t4 = millis();
    printf("Tiempo con threads: %ld ms\n", t4 - t3);

    return 0;
}
