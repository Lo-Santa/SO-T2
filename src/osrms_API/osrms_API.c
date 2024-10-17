#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include "osrms_API.h"

void signal_handler(int signal) {
    if (signal == SIGINT) {
        printf("\n");
        os_close();
        exit(0);
    }
}

void osrms_api() {
    char input[50];

    signal(SIGINT, signal_handler);

    while (1) {
        printf("Ingrese el nombre de la función: ");
        fgets(input, 50, stdin);

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            printf("Saliendo del programa.\n");
            os_close();
            break;
        }

        else if (strcmp(input, "os_ls_processes") == 0)
        {
            os_ls_processes();
        }
        else if (strcmp(input, "os_exists") == 0)
        {
            char process_input[10];
            int process_id;
            char file_name[15];

            printf("Ingrese el ID del proceso: ");
            fgets(process_input, 10, stdin);
            process_id = atoi(process_input);

            printf("Ingresa el nombre del archivo: ");
            fgets(file_name, sizeof(file_name), stdin);

            file_name[strcspn(file_name, "\n")] = '\0';
            
            int output = os_exists(process_id, file_name);

            if (output == 1)
            {
                printf("Archivo encontrado\n");
            }
            else
            {
                printf("Archivo no existe dentro de proceso %d\n", process_id);
            }
        }

        else if (strcmp(input, "os_ls_files") == 0) {
            char process_input[10];
            int process_id;

            printf("Ingrese el ID del proceso: ");
            fgets(process_input, 10, stdin);
            process_id = atoi(process_input);

            os_ls_files(process_id);
        }

        else if (strcmp(input, "os_frame_bitmap") == 0) {
            os_frame_bitmap();
        }
        
        else if (strcmp(input, "os_tp_bitmap") == 0) {
            os_tp_bitmap();
        }

        else if (strcmp(input, "os_start_process") == 0)
        {
            char process_input[10];
            int process_id;
            char process_name[20];

            printf("Ingrese el ID del proceso: ");
            fgets(process_input, 10, stdin);
            process_id = atoi(process_input);

            while (1)
            {
                printf("Ingresa un nombre (máximo 11 caracteres): ");
                fgets(process_name, sizeof(process_name), stdin);

                process_name[strcspn(process_name, "\n")] = '\0';

                if (strlen(process_name) > 11) {
                    printf("El nombre es demasiado largo. Intenta nuevamente.\n");
                } else {
                    break;
                }
            }
            os_start_process(process_id, process_name);
        }

        else if (strcmp(input, "os_finish_process) == 0"))
        {
            char process_input[10];
            int process_id;

            printf("Ingrese el ID del proceso: ");
            fgets(process_input, 10, stdin);
            process_id = atoi(process_input);

            os_finish_process(process_id);
        }
        

        else {
            printf("Función no reconocida. Por favor, intente nuevamente.\n");
        }
    }
}