#include "../include/kitchen.h"

int main() {
    // Obtener acceso a la memoria compartida existente
    key_t key = ftok(".", 'K');
    int shm_id = shmget(key, sizeof(SharedMemory), 0666);
    if (shm_id == -1) {
        printf("Error: No se encontró el sistema de cocina en ejecución\n");
        printf("Asegúrate de que kitchen esté corriendo primero\n");
        exit(1);
    }

    // Obtener acceso al semáforo existente
    int sem_id = semget(key, 1, 0666);
    if (sem_id == -1) {
        printf("Error: No se encontró el semáforo del sistema\n");
        exit(1);
    }

    SharedMemory* memory = shmat(shm_id, NULL, 0);
    if (memory == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    printf("\033[2J\033[H"); // Limpiar pantalla
    printf("=== Monitor de Cocina ===\n");
    printf("Presiona Ctrl+C para salir\n\n");

    while (1) {
        sem_lock(sem_id);
        
        if (memory->should_terminate) {
            printf("\nEl sistema de cocina está terminando...\n");
            sem_unlock(sem_id);
            break;
        }

        // Mostrar estado actual con colores y barras de progreso
        printf("\033[H"); // Mover cursor al inicio
        printf("=== Estado del Stock ===\n");
        for (int i = 0; i < NUM_INGREDIENTS; i++) {
            int stock = memory->stock[i];
            float percentage = (float)stock / MAX_STOCK * 100;
            
            // Elegir color basado en nivel de stock
            if (stock < MIN_STOCK) {
                printf("\033[31m"); // Rojo
            } else if (stock < MAX_STOCK/2) {
                printf("\033[33m"); // Amarillo
            } else {
                printf("\033[32m"); // Verde
            }
            
            printf("%-10s: %2d [", INGREDIENT_NAMES[i], stock);
            
            // Crear barra de progreso
            int bars = (int)(percentage / 5);
            for (int j = 0; j < 20; j++) {
                if (j < bars) {
                    printf("█");
                } else {
                    printf(" ");
                }
            }
            printf("] %3.0f%%\033[0m\n", percentage);
        }

        sem_unlock(sem_id);
        sleep(1);
    }

    if (shmdt(memory) == -1) {
        perror("shmdt failed");
    }

    return 0;
} 