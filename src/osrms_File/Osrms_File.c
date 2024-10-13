#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Osrms_File.h"

// Variable global para el archivo de memoria
FILE *memory_file = NULL;

// Estructura para la tabla de archivos dentro del PCB
typedef struct {
    unsigned char valid;      // 0x01 si es válido, 0x00 si no
    char file_name[14];       // Nombre del archivo
    unsigned int file_size;   // Tamaño del archivo
} FileEntry;


// Función os_mount para montar el archivo de memoria
void os_mount(char* memory_path) {
    // Intentar abrir el archivo en modo binario de lectura/escritura
    memory_file = fopen(memory_path, "rb+");
    if (memory_file == NULL) {
        perror("Error al abrir el archivo de memoria");
    } else {
        printf("Memoria montada correctamente: %s\n", memory_path);
    }
}

// Función os_close para cerrar el archivo al terminar el programa.
void os_close()
{
    if (memory_file != NULL) {
        fclose(memory_file);
        printf("Archivo de memoria cerrado correctamente.\n");
    }
}

// Función para listar archivos de un proceso
void os_ls_files(int process_id) {

    int found_process = 0;  // Bandera para ver si encontramos el proceso

    // Paso 1: Buscar el proceso en la tabla de PCBs
    for (int i = 0; i < MAX_PROCESSES; i++) {
        // Posicionarse en la entrada del PCB i-ésimo
        size_t pcb_offset = PCB_START + (i * PCB_SIZE);
        fseek(memory_file, pcb_offset, SEEK_SET);

        int current_process_id;

        // Leer el process_id del proceso (1 byte de estado y 1 byte de id)
        fseek(memory_file, 1, SEEK_CUR);  // Saltar el byte del estado
        fread(&current_process_id, 1, 1, memory_file);

        // Si el ID del proceso coincide con el solicitado
        if (current_process_id == process_id) {
            found_process = 1;
            printf("Proceso %d encontrado. Listando archivos:\n", process_id);

            // Busqueda de la tabla de archivos, saltar 1 byte de id, 1 byte de estado y 11 bytes de nombre de proceso
            fseek(memory_file, pcb_offset + 2 + PCB_NAME_SIZE, SEEK_SET);

            // Paso 2: Leer la tabla de archivos del proceso
            for (int j = 0; j < MAX_FILES; j++) {
                FileEntry file_entry;

                // Leer la entrada de archivo
                fread(&file_entry.valid, 1, 1, memory_file);

                if (file_entry.valid == 0x01) {
                    // Leer el nombre y el tamaño del archivo
                    fread(file_entry.file_name, 1, 14, memory_file);
                    fread(&file_entry.file_size, 4, 1, memory_file);

                    // Imprimir la información del archivo
                    printf("  Archivo: %s, Tamaño: %u bytes\n", file_entry.file_name, file_entry.file_size);
                } else {
                    // Si el archivo no es válido, saltar el resto de la entrada
                    fseek(memory_file, 18, SEEK_CUR);  // Saltar el nombre (14 bytes) y el tamaño (4 bytes)
                }
            }
            printf("\n");
            break;  // Ya encontramos el proceso, no es necesario seguir buscando
        }
    }

    if (!found_process) {
        printf("Proceso con ID %d no encontrado.\n", process_id);
    }
}

// Función para imprimir el estado del Frame Bitmap y contar los frames ocupados y libres
void os_frame_bitmap() {
    // Desplazarse hasta el inicio del Frame Bitmap
    fseek(memory_file, FRAME_BITMAP_OFFSET, SEEK_SET);

    // Buffer para almacenar los 8 KB del Frame Bitmap
    unsigned char bitmap[FRAME_BITMAP_SIZE];

    // Leer el Frame Bitmap desde la memoria
    size_t bytes_read = fread(bitmap, 1, FRAME_BITMAP_SIZE, memory_file);
    if (bytes_read != FRAME_BITMAP_SIZE) {
        perror("Error al leer el Frame Bitmap");
        fclose(memory_file);
        return;
    }

    int frames_ocupados = 0;
    int frames_libres = 0;

    // Recorrer cada byte del bitmap
    for (int i = 0; i < FRAME_BITMAP_SIZE; i++) {
        for (int bit = 0; bit < 8; bit++) {
            int estado_frame = (bitmap[i] >> bit) & 1;  // Obtener el estado del frame (ocupado o libre)
            if (estado_frame == 1) {
                frames_ocupados++;
            } else {
                frames_libres++;
            }
        }
    }

    // Imprimir el resultado
    printf("Estado del Frame Bitmap (en hexadecimal):\n");
    for (int i = 0; i < FRAME_BITMAP_SIZE; i++) {
        if (i % 16 == 0) printf("\n");  // Imprimir 16 bytes por línea
        printf("%02x ", bitmap[i]);
    }
    printf("\n\nFrames ocupados: %d\n", frames_ocupados);
    printf("Frames libres: %d\n\n", frames_libres);
}


// Función para imprimir el estado del Bitmap de Tablas de Páginas y contar las tablas ocupadas y libres
void os_tp_bitmap() {
    // Desplazarse hasta el inicio del Bitmap de Tablas de Páginas
    fseek(memory_file, TP_BITMAP_OFFSET, SEEK_SET);

    // Buffer para almacenar los 128 bytes del Bitmap
    unsigned char bitmap[TP_BITMAP_SIZE];

    // Leer el Bitmap de Tablas de Páginas desde la memoria
    size_t bytes_read = fread(bitmap, 1, TP_BITMAP_SIZE, memory_file);
    if (bytes_read != TP_BITMAP_SIZE) {
        perror("Error al leer el Bitmap de Tablas de Páginas");
        fclose(memory_file);
        return;
    }

    int tablas_ocupadas = 0;
    int tablas_libres = 0;

    // Recorrer cada byte del bitmap
    for (int i = 0; i < TP_BITMAP_SIZE; i++) {
        for (int bit = 0; bit < 8; bit++) {
            int estado_tabla = (bitmap[i] >> bit) & 1;  // Obtener el estado de la tabla de páginas (ocupada o libre)
            if (estado_tabla == 1) {
                tablas_ocupadas++;
            } else {
                tablas_libres++;
            }
        }
    }

    // Imprimir el estado del Bitmap de Tablas de Páginas en hexadecimal
    printf("Estado del Bitmap de Tablas de Páginas (en hexadecimal):\n");
    for (int i = 0; i < TP_BITMAP_SIZE; i++) {
        if (i % 16 == 0) printf("\n");  // Imprimir 16 bytes por línea
        printf("%02x ", bitmap[i]);
    }
    printf("\n\nTablas ocupadas: %d\n", tablas_ocupadas);
    printf("Tablas libres: %d\n\n", tablas_libres);
}
