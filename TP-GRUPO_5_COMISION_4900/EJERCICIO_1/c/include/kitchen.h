#ifndef KITCHEN_H
#define KITCHEN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <time.h>

// Para macOS/Linux compatibility
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define SHM_SIZE 1024
#define NUM_INGREDIENTS 8
#define MAX_STOCK 15
#define MIN_STOCK 5
#define MAX_CHILDREN 6  // máximo: 4 cocineros especializados + 1 reponedor + 1 monitor
#define NUM_RECIPES 4
#define MAX_RECIPE_INGREDIENTS 6

// Índices de ingredientes
enum Ingredients {
    PAN,
    CARNE,
    LECHUGA,
    TOMATE,
    QUESO,
    TORTILLA,
    SALSA,
    ZANAHORIA
};

// Estructura para una receta
typedef struct {
    char name[50];
    int ingredients[NUM_INGREDIENTS];  // cantidad de cada ingrediente necesario
    int prep_time;  // tiempo de preparación en segundos
} Recipe;

// Estructura para almacenar el estado de la cocina
typedef struct {
    int stock[NUM_INGREDIENTS];
    int should_terminate;
    int recipes_to_prepare[NUM_RECIPES];  // cantidad a preparar de cada receta
    int recipes_completed[NUM_RECIPES];   // cantidad completada de cada receta
    int cooking_in_progress[NUM_RECIPES]; // cantidad en preparación de cada receta
    int total_completed;                  // total de platos completados
    int total_to_prepare;                 // total de platos a preparar
    int replenish_needed;                 // ✨ Nueva bandera para notificación de reposición
} SharedMemory;

// Nombres de los ingredientes
extern const char* INGREDIENT_NAMES[NUM_INGREDIENTS];

// Recetas disponibles
extern const Recipe RECIPES[NUM_RECIPES];

// Funciones principales
void specialized_cook_process(int shm_id, int sem_id, int cook_id, int recipe_index);
void replenisher_process(int shm_id, int sem_id);
void monitor_process(int shm_id, int sem_id);
void init_shared_memory(SharedMemory* memory, int recipes_count[NUM_RECIPES]);

// Funciones de utilidad
void sem_lock(int sem_id);
void sem_unlock(int sem_id);
void cleanup_resources(int shm_id, int sem_id);
void handle_sigterm(int signum);

// ✨ Nuevas funciones para supervisión de procesos
void handle_sigchld(int signum);
pid_t check_dead_children();
void terminate_all_children();

// Funciones de recetas
int can_cook_recipe(SharedMemory* memory, int recipe_index);
void consume_ingredients_for_recipe(SharedMemory* memory, int recipe_index);
void display_recipes();
int get_next_recipe_to_cook(SharedMemory* memory);
void print_usage(const char* program_name);
int parse_arguments(int argc, char* argv[], int recipes_count[NUM_RECIPES]);
int count_active_recipes(int recipes_count[NUM_RECIPES]);

// Variables globales para manejo de señales y semáforos
extern int g_shm_id;
extern int g_sem_id;
extern int g_sem_replenish;  // ✨ Nuevo semáforo para notificación de reposición

// ✨ Variables globales para supervisión de procesos
extern volatile sig_atomic_t g_child_died;
extern pid_t* g_child_pids;
extern int g_child_count;

#endif 