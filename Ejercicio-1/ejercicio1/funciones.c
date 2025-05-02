#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include "funciones.h"

int semaforo_id;

/// crear memoria compartida y devolver su ID para poder conectarla
int crear_memoria_compartida() {
    int id = shmget(IPC_PRIVATE, sizeof(DatosCompartidos), IPC_CREAT | 0666);
    if (id == -1) {
        perror("Error al crear memoria compartida");
        exit(1);
    }
    return id; // crea una memoria nueva, reserva espacio necesario
}

/// conectar el proceso a la memoria compartida usando el ID. devolver un puntero que se puede usar para leer o escribir los datos
DatosCompartidos* conectar_memoria(int id_memoria) {
    DatosCompartidos* datos = (DatosCompartidos*) shmat(id_memoria, NULL, 0);
    if (datos == (void*) -1) {
        perror("Error al conectar memoria");
        exit(1);
    }
    return datos;
}

/// desconecta y elimina la memoria compartida
void desconectar_y_eliminar_memoria(int id_memoria, DatosCompartidos* datos) {
    shmdt(datos);                  // desconecta la memoria
    shmctl(id_memoria, IPC_RMID, NULL); // eliminar del sistema
}

/// inicializa un semáforo en 1 (que está libre)
void iniciar_semaforo() {
    semaforo_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semaforo_id < 0) {
        perror("Error al crear semáforo");
        exit(1);
    }
    if (semctl(semaforo_id, 0, SETVAL, 1) == -1) {
        perror("Error al inicializar semáforo");
        exit(1);
    }
}

/// espera semáforo (operación PEDIR)
void esperar_semaforo() {
    struct sembuf operacion = {0, -1, 0}; // baja el semáforo una posición y si está en 1, entra. Si está en 0, queda bloqueado.
    if (semop(semaforo_id, &operacion, 1) == -1) {
        perror("Error al esperar semáforo");
        exit(1);
    }
}

/// libera semáforo (operación LIBERAR)
void liberar_semaforo() {
    struct sembuf operacion = {0, 1, 0}; // sube el semáforo en 1 y permite que otro proceso entre.
    if (semop(semaforo_id, &operacion, 1) == -1) {
        perror("Error al liberar semáforo");
        exit(1);
    }
}

/// elimina semáforo del sistema
void eliminar_semaforo() {
    if (semctl(semaforo_id, 0, IPC_RMID) == -1) {
        perror("Error al eliminar semáforo");
        exit(1);
    }
}

/// operaciones hijos
void tarea_hijo(int id, DatosCompartidos* datos) {
    printf("Hijo %d: iniciado\n", id);

    while (!datos->finalizar) {
        esperar_semaforo();  // sección crítica

        printf("Hijo %d: entrando a sección crítica\n", id);

        for (int i = 0; i < TAMANO_VECTOR; i++) {
            int antes = datos->vector[i];  // guardar valor original
            switch (id) {
                case 0: datos->vector[i] += 1; break;
                case 1: datos->vector[i] -= 1; break;
                case 2: datos->vector[i] *= 2; break;
            }
            printf("Hijo %d: vector[%d] = %d -> %d\n", id, i, antes, datos->vector[i]);
        }

        printf("Hijo %d: saliendo de sección crítica\n", id);

        liberar_semaforo();
        sleep(10);
    }

    printf("Hijo %d finalizó\n", id);
    exit(0);
}
