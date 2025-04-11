#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include "funciones.h"

int semaforo_id;

/// crear memoria compartida y devolver su ID para poder conectarla

int crear_memoria_compartida() {
    return shmget(IPC_PRIVATE, sizeof(DatosCompartidos), IPC_CREAT | 0666);
        // crea una memoria nueva , reserva espacio necesario
}

/// conectar el proceso a la memoria compartida usando el ID. devolver un puntero que se puede usar para leer o escribir los datos

DatosCompartidos* conectar_memoria(int id_memoria) {
    return (DatosCompartidos*) shmat(id_memoria, NULL, 0);
}

/// desconecta y elimina la memoria compartida

void desconectar_y_eliminar_memoria(int id_memoria, DatosCompartidos* datos) {
    shmdt(datos);                  // desconecta la memoria
    shmctl(id_memoria, IPC_RMID, NULL); // eliminar del sistema
}

/// inicializa un semáforo en 1 (que esta libre)

void iniciar_semaforo() {
    semaforo_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semaforo_id < 0) {
        perror("Error al crear semáforo");
        exit(1);
    }
    semctl(semaforo_id, 0, SETVAL, 1);
}

/// espera semáforo (operación PEDIR)

void esperar_semaforo() {
    struct sembuf operacion = {0, -1, 0}; //baja el semaforo una posicion y si esta en 1, entra. 0 queda bloqueado.
    semop(semaforo_id, &operacion, 1);
}

/// libera semáforo (operación LIBERAR)

void liberar_semaforo() {
    struct sembuf operacion = {0, 1, 0}; //sube el semaforo en 1 y permite que otro proceso entre.
    semop(semaforo_id, &operacion, 1);
}

/// elimina semáforo del sistema

void eliminar_semaforo() {
    semctl(semaforo_id, 0, IPC_RMID);
}

/// operaciones hijos

void tarea_hijo(int id, DatosCompartidos* datos) {
    while (!datos->finalizar) {  //el hijo va a estar trabajando en un bucle infinito hasta que finalizar valga 1 (lo decide el proceso padre).
        esperar_semaforo();  // seccion critica, bloquea a todos mientras trabaja

        for (int i = 0; i < TAMANO_VECTOR; i++) {
            switch (id) {
                case 0: datos->vector[i] += 1; break; // suma uno a cada elemento
                case 1: datos->vector[i] -= 1; break; // resta 1
                case 2: datos->vector[i] *= 2; break; // multiplica por 2
            }
        }

        liberar_semaforo();  // Sale de la sección crítica
        sleep(15); // Simula trabajo de 15 seegundos
    }

    printf("Hijo %d finalizó\n", id);
    exit(0);
}
