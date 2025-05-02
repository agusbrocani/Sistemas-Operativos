#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include <sys/types.h>

#define TAMANO_VECTOR 5

/// estructura que se comparte en memoria
typedef struct {
    int vector[TAMANO_VECTOR]; // cantidad de hijos para trabajar
    int finalizar; // bandera para saber si el proceso debe continuar o terminar (0,1)
} DatosCompartidos;

/// funciones para manejar memoria compartida
int crear_memoria_compartida(); // crea memoria compartida
DatosCompartidos* conectar_memoria(int id_memoria); // conecta el proceso al bloque de memoria con el id de la func anterior
void desconectar_y_eliminar_memoria(int id_memoria, DatosCompartidos* datos); // bajar los datos de la memoria

/// funciones para controlar los semáforos
void iniciar_semaforo(); // crea un semáforo en 1 (usable)
void esperar_semaforo(); // pedir semáforo
void liberar_semaforo(); // liberar semáforo
void eliminar_semaforo(); // elimina del sistema

/// función para indicar al hijo qué hacer
void tarea_hijo(int id, DatosCompartidos* datos);
// id -> hijo
// datos -> estructura de memoria compartida

#endif // FUNCIONES_H_INCLUDED
