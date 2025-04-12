#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define N_TAREAS 10

void simular_api(int id) {
    printf("Tarea %d esperando respuesta...\n", id);
    sleep(1); // Espera simulada
    printf("Tarea %d recibió respuesta.\n", id);
}

typedef struct {
    int id;
} hilo_args;

void* hilo_func(void* arg) {
    hilo_args* args = (hilo_args*)arg;
    simular_api(args->id);
    pthread_exit(NULL);
}

// === Timer ===

long millis() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

int main() {
    pthread_t threads[N_TAREAS];
    hilo_args args[N_TAREAS];

    // === SECUENCIAL ===
    long t1 = millis();
    for (int i = 0; i < N_TAREAS; i++) {
        simular_api(i);
    }
    long t2 = millis();
    printf("Tiempo secuencial: %ld ms\n\n", t2 - t1);

    // === CON THREADS ===
    long t3 = millis();
    for (int i = 0; i < N_TAREAS; i++) {
        args[i].id = i;
        pthread_create(&threads[i], NULL, hilo_func, &args[i]);
    }
    for (int i = 0; i < N_TAREAS; i++) {
        pthread_join(threads[i], NULL);
    }
    long t4 = millis();
    printf("Tiempo con threads: %ld ms\n", t4 - t3);

    return 0;
}
