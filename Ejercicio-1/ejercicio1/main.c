#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "funciones.h"

#define N 3 /// cantidad de hijos que vamos a crear, en este caso elegi 3

int main() {

    /// creo memoria compartida

    int id_memoria = crear_memoria_compartida(); //zona memoria compartida
    DatosCompartidos* datos = conectar_memoria(id_memoria); //la conecta a nuestro proceso y devuelve un puntero
    //que usamos como si fuera una variable normal.


    /// inicializo datos

    for (int i = 0; i < TAMANO_VECTOR; i++) datos->vector[i] = i + 1; //lleno el vector con valores 1 a 5

    datos->finalizar = 0;  //indica que los hijos no deben terminar.

    ///creo el semaforo

    iniciar_semaforo(); // creo un semaforo con valor 1. lo uso para que solo un proceso hijo a la vez pueda modificar el vector.

    ///creo un vector donde guardo los pid de los hijos

    pid_t hijos[N];

    /// creo proceso e hijos

    for (int i = 0; i < N; i++) {
        pid_t pid = fork(); //creo un nuevo proceso
        if (pid == 0) { //estamos dentro del hijo, asi que llamamos a tarea_hijo() con su número e inmediatamente empieza su trabajo.
            tarea_hijo(i, datos);
        } else if (pid > 0) { //estamos en el padre, y guardamos el PID del hijo.
            hijos[i] = pid;
        } else {        //hubo un error al crear el proceso.
            perror("Error al crear hijo");
            exit(1);
        }
    }

    printf("Presione ENTER para finalizar...\n");
    getchar();

    // señal de finalización, modifica el campo finalizar de la memoria compartida. Los hijos, al ver esto, salen del bucle y terminan.
    datos->finalizar = 1;

    // el proceso padre espera que los hijos terminen su ejecución con wait().
    for (int i = 0; i < N; i++) wait(NULL);

    // una vez que los hijos terminaron, el padre imprime el vector modificado por los procesos.
    printf("Resultado final del vector:\n");
    for (int i = 0; i < TAMANO_VECTOR; i++) {
        printf("%d ", datos->vector[i]);
    }
    printf("\n");

    eliminar_semaforo();
    desconectar_y_eliminar_memoria(id_memoria, datos);

    return 0;
}
