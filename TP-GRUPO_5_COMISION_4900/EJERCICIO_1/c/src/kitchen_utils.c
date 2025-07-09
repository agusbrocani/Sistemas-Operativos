#include "../include/kitchen.h"
#include <errno.h>

// Variables globales para manejo de señales
int g_shm_id = -1;
int g_sem_id = -1;
int g_sem_replenish = -1;  // ✨ Nuevo semáforo para notificación de reposición

// ✨ Nuevas variables para supervisión de procesos
volatile sig_atomic_t g_child_died = 0;  // Bandera para SIGCHLD
pid_t* g_child_pids = NULL;              // Array de PIDs de procesos hijo
int g_child_count = 0;                   // Número de procesos hijo

// Nombres de los ingredientes
const char* INGREDIENT_NAMES[NUM_INGREDIENTS] = {
    "Pan", "Carne", "Lechuga", "Tomate", "Queso", "Tortilla", "Salsa", "Zanahoria"
};

// Recetas disponibles
const Recipe RECIPES[NUM_RECIPES] = {
    {
        .name = "Hamburguesa",
        .ingredients = {2, 1, 1, 1, 1, 0, 0, 0}, // Pan(2), Carne(1), Lechuga(1), Tomate(1), Queso(1)
        .prep_time = 3
    },
    {
        .name = "Taco",
        .ingredients = {0, 1, 1, 1, 1, 1, 1, 0}, // Carne(1), Lechuga(1), Tomate(1), Queso(1), Tortilla(1), Salsa(1)
        .prep_time = 2
    },
    {
        .name = "Sandwich Vegetal",
        .ingredients = {2, 0, 2, 2, 1, 0, 0, 1}, // Pan(2), Lechuga(2), Tomate(2), Queso(1), Zanahoria(1)
        .prep_time = 2
    },
    {
        .name = "Quesadilla",
        .ingredients = {0, 0, 0, 0, 2, 2, 1, 0}, // Queso(2), Tortilla(2), Salsa(1)
        .prep_time = 1
    }
};

// Funciones de semáforos
void sem_lock(int sem_id) {
    struct sembuf sb = {0, -1, 0};
    if (semop(sem_id, &sb, 1) == -1) {
        perror("sem_lock failed");
        exit(1);
    }
}

void sem_unlock(int sem_id) {
    struct sembuf sb = {0, 1, 0};
    if (semop(sem_id, &sb, 1) == -1) {
        perror("sem_unlock failed");
        exit(1);
    }
}

// Función para inicializar la memoria compartida
void init_shared_memory(SharedMemory* memory, int recipes_count[NUM_RECIPES]) {
    for (int i = 0; i < NUM_INGREDIENTS; i++) {
        memory->stock[i] = MAX_STOCK;
    }
    memory->should_terminate = 0;
    memory->total_completed = 0;
    memory->total_to_prepare = 0;
    memory->replenish_needed = 0;  // ✨ Inicializar nueva bandera
    
    for (int i = 0; i < NUM_RECIPES; i++) {
        memory->recipes_to_prepare[i] = recipes_count[i];
        memory->recipes_completed[i] = 0;
        memory->cooking_in_progress[i] = 0;
        memory->total_to_prepare += recipes_count[i];
    }
}

// Verificar si se puede cocinar una receta
int can_cook_recipe(SharedMemory* memory, int recipe_index) {
    if (recipe_index < 0 || recipe_index >= NUM_RECIPES) {
        return 0;
    }
    
    // Verificar si aún quedan por preparar de esta receta
    if (memory->recipes_completed[recipe_index] + memory->cooking_in_progress[recipe_index] 
        >= memory->recipes_to_prepare[recipe_index]) {
        return 0;
    }
    
    // Verificar ingredientes disponibles
    for (int i = 0; i < NUM_INGREDIENTS; i++) {
        if (memory->stock[i] < RECIPES[recipe_index].ingredients[i]) {
            return 0;
        }
    }
    return 1;
}

// Consumir ingredientes para una receta
void consume_ingredients_for_recipe(SharedMemory* memory, int recipe_index) {
    if (recipe_index < 0 || recipe_index >= NUM_RECIPES) {
        return;
    }
    
    int need_replenish = 0;
    
    // Consumir ingredientes
    for (int i = 0; i < NUM_INGREDIENTS; i++) {
        memory->stock[i] -= RECIPES[recipe_index].ingredients[i];
        
        // ✨ Verificar si algún ingrediente quedó bajo
        if (memory->stock[i] < MIN_STOCK) {
            need_replenish = 1;
        }
    }
    memory->cooking_in_progress[recipe_index]++;
    
    // ✨ Notificar al reponedor si es necesario
    if (need_replenish && !memory->replenish_needed) {
        memory->replenish_needed = 1;
        
        // Señalar al reponedor (incrementar semáforo de notificación)
        if (g_sem_replenish != -1) {
            struct sembuf sb = {0, 1, 0}; // semáforo índice 0, +1
            if (semop(g_sem_replenish, &sb, 1) == -1) {
                perror("semop notify replenisher failed");
            } else {
                printf("🔔 Cocinero notificó necesidad de reposición\n");
            }
        }
    }
}

// Mostrar recetas disponibles
void display_recipes() {
    printf("\n=== RECETAS DISPONIBLES ===\n");
    for (int i = 0; i < NUM_RECIPES; i++) {
        printf("%d. %s (Tiempo: %ds)\n", i + 1, RECIPES[i].name, RECIPES[i].prep_time);
        printf("   Ingredientes: ");
        int first = 1;
        for (int j = 0; j < NUM_INGREDIENTS; j++) {
            if (RECIPES[i].ingredients[j] > 0) {
                if (!first) printf(", ");
                printf("%s(%d)", INGREDIENT_NAMES[j], RECIPES[i].ingredients[j]);
                first = 0;
            }
        }
        printf("\n");
    }
    printf("===========================\n\n");
}

// Obtener la próxima receta a cocinar (busca la que tenga más pendientes)
int get_next_recipe_to_cook(SharedMemory* memory) {
    int best_recipe = -1;
    int max_pending = 0;
    
    for (int i = 0; i < NUM_RECIPES; i++) {
        int pending = memory->recipes_to_prepare[i] - 
                     memory->recipes_completed[i] - 
                     memory->cooking_in_progress[i];
        
        if (pending > max_pending && can_cook_recipe(memory, i)) {
            max_pending = pending;
            best_recipe = i;
        }
    }
    
    return best_recipe;
}

// Mostrar uso del programa
void print_usage(const char* program_name) {
    printf("Uso: %s <hamburguesas> <tacos> <sandwiches> <quesadillas>\n", program_name);
    printf("Ejemplo: %s 5 3 2 4\n", program_name);
    printf("  - Preparará 5 hamburguesas, 3 tacos, 2 sandwiches y 4 quesadillas\n\n");
    display_recipes();
}

// Parsear argumentos de línea de comandos
int parse_arguments(int argc, char* argv[], int recipes_count[NUM_RECIPES]) {
    if (argc != 5) {
        return 0; // Error: número incorrecto de argumentos
    }
    
    for (int i = 0; i < NUM_RECIPES; i++) {
        recipes_count[i] = atoi(argv[i + 1]);
        if (recipes_count[i] < 0) {
            printf("Error: Las cantidades deben ser números positivos\n");
            return 0;
        }
    }
    
    return 1; // Éxito
}

// Contar cuántas recetas tienen pedidos (cantidad > 0)
int count_active_recipes(int recipes_count[NUM_RECIPES]) {
    int active_count = 0;
    for (int i = 0; i < NUM_RECIPES; i++) {
        if (recipes_count[i] > 0) {
            active_count++;
        }
    }
    return active_count;
}

// Limpieza de recursos mejorada
void cleanup_resources(int shm_id, int sem_id) {
    printf("Limpiando recursos del sistema...\n");
    
    // Limpiar memoria compartida
    if (shm_id != -1) {
        if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
            perror("shmctl IPC_RMID failed");
        } else {
            printf("✓ Memoria compartida liberada (ID: %d)\n", shm_id);
        }
    }
    
    // Limpiar semáforo principal
    if (sem_id != -1) {
        if (semctl(sem_id, 0, IPC_RMID) == -1) {
            perror("semctl IPC_RMID failed");
        } else {
            printf("✓ Semáforo principal liberado (ID: %d)\n", sem_id);
        }
    }
    
    // ✨ Limpiar semáforo de reposición
    if (g_sem_replenish != -1) {
        if (semctl(g_sem_replenish, 0, IPC_RMID) == -1) {
            perror("semctl replenish IPC_RMID failed");
        } else {
            printf("✓ Semáforo de reposición liberado (ID: %d)\n", g_sem_replenish);
        }
    }
}

// Manejador de señales mejorado
void handle_sigterm(int signum) {
    printf("\nRecibida señal de terminación\n");
    cleanup_resources(g_shm_id, g_sem_id);
    exit(0);
}

// ✨ Nuevo manejador para muerte de procesos hijo
void handle_sigchld(int signum) {
    g_child_died = 1;  // Marcar que un hijo murió
}

// ✨ Función para verificar qué proceso murió
pid_t check_dead_children() {
    int status;
    pid_t dead_pid;
    
    // Verificar todos los hijos sin bloquear
    while ((dead_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Verificar si el proceso terminó anormalmente
        if (WIFSIGNALED(status)) {
            printf("🚨 Proceso hijo %d terminó por señal %d (%s)\n", 
                   dead_pid, WTERMSIG(status), strsignal(WTERMSIG(status)));
            return dead_pid;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            printf("🚨 Proceso hijo %d terminó con error (código: %d)\n", 
                   dead_pid, WEXITSTATUS(status));
            return dead_pid;
        }
        // Si terminó normalmente (WEXITSTATUS == 0), continuar
    }
    
    return 0;  // No hay procesos muertos anormalmente
}

// ✨ Función para terminar todos los procesos hijo
void terminate_all_children() {
    printf("🛑 Terminando todos los procesos hijo...\n");
    
    for (int i = 0; i < g_child_count; i++) {
        if (g_child_pids[i] > 0) {
            if (kill(g_child_pids[i], 0) == 0) {  // Verificar si el proceso existe
                printf("   Terminando proceso %d\n", g_child_pids[i]);
                kill(g_child_pids[i], SIGTERM);
            }
        }
    }
    
    // Esperar un momento para terminación limpia
    sleep(1);
    
    // Forzar terminación si es necesario
    for (int i = 0; i < g_child_count; i++) {
        if (g_child_pids[i] > 0) {
            if (kill(g_child_pids[i], 0) == 0) {  // Si aún existe
                printf("   Forzando terminación de proceso %d\n", g_child_pids[i]);
                kill(g_child_pids[i], SIGKILL);
            }
        }
    }
}