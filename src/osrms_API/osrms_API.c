#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include "osrms_API.h"

void signal_handler(int signal) {
    if (signal == SIGINT) {
        os_close();  // Cerrar el archivo correctamente
        exit(0);     // Salir del programa de forma segura
    }
}

void osrms_api() {
    char input[50];  // Buffer para almacenar la entrada del usuario

    signal(SIGINT, signal_handler);

    while (1) {
        // Pedir al usuario que ingrese el nombre de la función o 'exit' para salir
        printf("Ingrese el nombre de la función (os_ls_files, os_frame_bitmap, o 'exit' para salir): ");
        fgets(input, 50, stdin);  // Leer la entrada del usuario

        // Eliminar el salto de línea al final de la entrada
        input[strcspn(input, "\n")] = 0;

        // Si el usuario escribe 'exit', terminamos el ciclo
        if (strcmp(input, "exit") == 0) {
            printf("Saliendo del programa.\n");
            os_close();
            break;
        }

        // Comprobar si el usuario quiere ejecutar 'os_ls_files'
        if (strcmp(input, "os_ls_files") == 0) {
            char process_input[10];
            int process_id;

            // Pedir el process_id
            printf("Ingrese el ID del proceso: ");
            fgets(process_input, 10, stdin);  // Leer el ID del proceso
            process_id = atoi(process_input);  // Convertir el input a entero

            // Llamar a la función os_ls_files con el ID del proceso
            os_ls_files(process_id);
        }

        // Comprobar si el usuario quiere ejecutar 'os_frame_bitmap'
        else if (strcmp(input, "os_frame_bitmap") == 0) {
            // Llamar a la función os_frame_bitmap
            os_frame_bitmap();
        }
        
        else if (strcmp(input, "os_tp_bitmap") == 0) {
            // Llamar a la función os_tp_bitmap
            os_tp_bitmap();
        }

        // Si la entrada no coincide con ninguna función válida
        else {
            printf("Función no reconocida. Por favor, intente nuevamente.\n");
        }
    }
}