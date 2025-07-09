#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

void print_header() {
    printf("\033[2J\033[H"); // Limpiar pantalla
    printf("=== Monitor de Procesos de Cocina ===\n\n");
    printf("Procesos activos:\n");
}

int main() {
    while(1) {
        print_header();
        
        // Ejecutar ps y filtrar procesos de cocina
        printf("\033[34m"); // Azul
        system("ps aux | grep -E 'kitchen|kitchen_monitor' | grep -v grep | awk '{printf \"%-10s %-8s %s\\n\", $11, $2, $10}'");
        printf("\033[0m"); // Reset color
        
        printf("\nMemoria Compartida:\n");
        printf("\033[33m"); // Amarillo
        system("ipcs -m | grep $(whoami) | awk '{printf \"ID: %-8s Tamaño: %-8s\\n\", $2, $5}'");
        printf("\033[0m"); // Reset color
        
        printf("\nSemáforos:\n");
        printf("\033[32m"); // Verde
        system("ipcs -s | grep $(whoami) | awk '{printf \"ID: %-8s\\n\", $2}'");
        printf("\033[0m"); // Reset color
        
        printf("\nPresiona Ctrl+C para salir\n");
        sleep(2);
    }
    
    return 0;
} 