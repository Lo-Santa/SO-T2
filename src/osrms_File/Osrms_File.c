#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Osrms_File.h"

// Variable global para el archivo de memoria
FILE *memory_file = NULL;

// Estructura para la tabla de archivos dentro del PCB
typedef struct {
    unsigned char valid;
    char file_name[14];
    unsigned int file_size;
} FileEntry;


// Función os_mount para montar el archivo de memoria
void os_mount(char* memory_path) {
    // Intentar abrir el archivo en modo binario de lectura/escritura
    memory_file = fopen(memory_path, "rb+");
    if (memory_file == NULL) {
        perror("Error al abrir el archivo de memoria");
        exit(0);
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

void os_ls_processes(){
    for (int i = 0; i< MAX_PROCESSES; i++){
        size_t pcb_offset = PCB_START + (i * PCB_ENTRY_SIZE);
        fseek(memory_file, pcb_offset, SEEK_SET);

        unsigned char current_process_status;
        unsigned char current_process_id;
        char name_process[12];

        fread(&current_process_status, 1, 1, memory_file);
        fread(&current_process_id, 1, 1, memory_file);
        fread(name_process, PROCESS_NAME_SIZE, 1, memory_file);
        name_process[11] = '\0';

        if (current_process_status == 0x01)
        {
            printf(" Proceso: %s, id: %d\n", name_process, current_process_id);
        }

    }
}

int os_exists(int process_id, char* file_name){
    int found_file = 0;

    // Paso 1: Buscar el proceso en la tabla de PCBs
    for (int i = 0; i < MAX_PROCESSES; i++) {
        // Posicionarse en la entrada del PCB i-ésimo
        size_t pcb_offset = PCB_START + (i * PCB_ENTRY_SIZE);
        fseek(memory_file, pcb_offset, SEEK_SET);

        unsigned char current_process_id;

        // Leer el process_id del proceso (1 byte de estado y 1 byte de id)
        fseek(memory_file, 1, SEEK_CUR);  // Saltar el byte del estado
        fread(&current_process_id, 1, 1, memory_file);

        // Si el ID del proceso coincide con el solicitado
        if (current_process_id == process_id) {
            printf("Proceso %d encontrado. BUscando archivo:\n", process_id);

            // Se salta directo a los 115bytes de la tabla de archivos
            fseek(memory_file, pcb_offset + 2 + PROCESS_NAME_SIZE, SEEK_SET);
            for (int j = 0; j<MAX_FILES; j++){
                FileEntry file_entry;

                fread(&file_entry.valid, 1, 1, memory_file);

                if (file_entry.valid == 0x01){
                    fread(file_entry.file_name, FILE_NAME_SIZE, 1, memory_file);
                    fread(&file_entry.file_size, FILE_SIZE, 1, memory_file);

                    if (strcmp(file_entry.file_name, file_name) == 0){
                        found_file = 1;
                        break;
                    }
                    fseek(memory_file, 4, SEEK_CUR);
                }

                else{
                    fseek(memory_file, 22, SEEK_CUR);
                }
            }
        }
    }

    if (!found_file)
    {
        printf("No existe proceso con ID %d\n", process_id);
        return 0;
    }
    else {
        return 1;
    }
    
}

void os_ls_files(int process_id) {

    int found_process = 0;

    // Paso 1: Buscar el proceso en la tabla de PCBs
    for (int i = 0; i < MAX_PROCESSES; i++) {
        size_t pcb_offset = PCB_START + (i * PCB_ENTRY_SIZE);
        fseek(memory_file, pcb_offset, SEEK_SET);

        unsigned char current_process_id;

        // Leer el process_id del proceso (1 byte de estado y 1 byte de id)
        fseek(memory_file, 1, SEEK_CUR);
        fread(&current_process_id, 1, 1, memory_file);

        if (current_process_id == process_id) {
            found_process = 1;
            printf("Proceso %d encontrado. Listando archivos:\n", current_process_id);

            // Busqueda de la tabla de archivos, saltar 1 byte de id, 1 byte de estado y 11 bytes de nombre de proceso
            fseek(memory_file, pcb_offset + 2 + PROCESS_NAME_SIZE, SEEK_SET);

            // Paso 2: Leer la tabla de archivos del proceso
            for (int j = 0; j < MAX_FILES; j++) {
                FileEntry file_entry;

                fread(&file_entry.valid, 1, 1, memory_file);

                if (file_entry.valid == 0x01) {
                    fread(file_entry.file_name, FILE_NAME_SIZE, 1, memory_file);
                    fread(&file_entry.file_size, FILE_SIZE, 1, memory_file);

                    printf("  Archivo: %s, Tamaño: %u Bytes\n", file_entry.file_name, file_entry.file_size);
                    fseek(memory_file, FILE_VPN_SIZE, SEEK_CUR);
                } else {
                    // Si el archivo no es válido, saltar el resto de la entrada
                    // Saltar el nombre (14 bytes), el tamaño del archivo (4 bytes) y la dirección virtual.
                    fseek(memory_file, FILE_NAME_SIZE + FILE_SIZE + FILE_VPN_SIZE, SEEK_CUR);
                }
            }
            printf("\n");
            break;
        }
    }

    if (!found_process) {
        printf("Proceso con ID %d no encontrado.\n", process_id);
    }
}

void print_binary(unsigned char byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
    }
    printf(" ");
}

void print_bitmap(int bitmap_size, int occupied, int free)
{
    unsigned char bitmap[bitmap_size];
    for (int i = 0; i < bitmap_size; i++) {
        if (i % 4 == 0) printf("\n");
        print_binary(bitmap[i]);
    }
    printf("\n\nOcupados: %d\n", occupied);
    printf("Libres: %d\n\n", free);
}

void os_frame_bitmap() {
    fseek(memory_file, FRAME_BITMAP_OFFSET, SEEK_SET);

    unsigned char bitmap[FRAME_BITMAP_SIZE];

    size_t bytes_read = fread(bitmap, 1, FRAME_BITMAP_SIZE, memory_file);
    if (bytes_read != FRAME_BITMAP_SIZE) {
        perror("Error al leer el Frame Bitmap");
        fclose(memory_file);
        return;
    }

    int frames_ocupados = 0;
    int frames_libres = 0;

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
    print_bitmap(FRAME_BITMAP_SIZE, frames_ocupados, frames_libres);
}

void os_tp_bitmap() {
    fseek(memory_file, TP_BITMAP_OFFSET, SEEK_SET);

    unsigned char bitmap[TP_BITMAP_SIZE];

    size_t bytes_read = fread(bitmap, 1, TP_BITMAP_SIZE, memory_file);
    if (bytes_read != TP_BITMAP_SIZE) {
        perror("Error al leer el Bitmap de Tablas de Páginas");
        fclose(memory_file);
        return;
    }

    int tablas_ocupadas = 0;
    int tablas_libres = 0;

    for (int i = 0; i < TP_BITMAP_SIZE; i++) {
        for (int bit = 0; bit < 8; bit++) {
            int estado_tabla = (bitmap[i] >> bit) & 1;
            if (estado_tabla == 1) {
                tablas_ocupadas++;
            } else {
                tablas_libres++;
            }
        }
    }

    print_bitmap(TP_BITMAP_SIZE, tablas_ocupadas, tablas_libres);
}

int is_pcb_entry_empty(size_t pcb_offset) {
    unsigned char buffer[PCB_ENTRY_SIZE];

    fseek(memory_file, pcb_offset, SEEK_SET);
    fread(buffer, 1, PCB_ENTRY_SIZE, memory_file);

    for (int i = 0; i < PCB_ENTRY_SIZE; i++) {
        if (buffer[i] != 0x00) {
            return 0;  // No está vacía
        }
    }
    return 1;  // Está vacía
}

void create_new_process(int entry_num, int process_id, char* process_name) {
    size_t process_offset = PCB_START + (entry_num * PCB_ENTRY_SIZE);
    fseek(memory_file, process_offset, SEEK_SET);

    unsigned char new_state = 0x01;
    fwrite(&new_state, 1, 1, memory_file);

    fwrite(&process_id, 1, 1, memory_file);

    char padded_name[PROCESS_NAME_SIZE] = {0};
    strncpy(padded_name, process_name, PROCESS_NAME_SIZE);
    fwrite(padded_name, 1, PROCESS_NAME_SIZE, memory_file);
}

void os_start_process(int process_id, char* process_name) {
    int process_found = 0;
    int empty_entry = -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        size_t pcb_offset = PCB_START + (i * PCB_ENTRY_SIZE);
        fseek(memory_file, pcb_offset, SEEK_SET);

        unsigned char process_state;
        unsigned char current_process_id;

        fread(&process_state, 1, 1, memory_file);
        fread(&current_process_id, 1, 1, memory_file);

        // Validación para no introducir un proceso con el mismo id que uno existente.
        if (current_process_id == process_id && process_state == 0x01)
        {
            printf("El proceso %d ya se encontraba en ejecución\n", process_id);
            process_found = 1;
            return;
        }
    //     // Si el proceso existe pero no está en ejecución
        else if (current_process_id == process_id && process_state == 0x00) {
            fseek(memory_file, pcb_offset, SEEK_SET);
            unsigned char new_state = 0x01;
            fwrite(&new_state, 1, 1, memory_file);

            printf("Proceso %d encontrado. Iniciando proceso nuevamente\n", process_id);
            process_found = 1;
            return;
        }

        if (empty_entry == -1)
        {
            if (is_pcb_entry_empty(pcb_offset)) {
                empty_entry = i;
            }
        }
    }

    // // Si no encontramos el proceso y hay una entrada vacía, creamos uno nuevo
    if (!process_found && empty_entry != -1) {
        create_new_process(empty_entry, process_id, process_name);
        printf("Nuevo proceso creado: ID %d, Nombre: %s\n", process_id, process_name);
    } else if (empty_entry == -1) {
        printf("Error: No hay espacio en la tabla de PCBs para crear un nuevo proceso.\n");
    }
}

void free_frame(uint16_t frame_number) {
    uint8_t byte;
    long frame_bitmap_offset = FRAME_BITMAP_OFFSET + (frame_number / 8);
    
    // Mover el puntero de archivo al byte correspondiente del frame bitmap
    fseek(memory_file, frame_bitmap_offset, SEEK_SET);
    fread(&byte, sizeof(uint8_t), 1, memory_file);
    
    // Limpiar el bit correspondiente para marcar el frame como libre
    byte &= ~(1 << (frame_number % 8));  // Usar AND con el bit en la posición correcta
    
    // Escribir el byte modificado de vuelta en el frame bitmap
    fseek(memory_file, frame_bitmap_offset, SEEK_SET);
    fwrite(&byte, sizeof(uint8_t), 1, memory_file);
}

void free_file(size_t process_entry, unsigned int file_size, unsigned char virtual_address) {
    // unsigned char VPN = (virtual_address >> 15) & 0xFFF;
    // unsigned char offset = virtual_address & 0x7FFF;
    // unsigned char first_table_index = (VPN >> 6) & 0x3F;
    // unsigned char second_table_index = VPN & 0x3F;
    
    // // Saltar a tabla de paginas  de primer orden, bytes hasta el proceso, 1 bytes de validez,
    // // 1 byte de id, 11 bytes de nombre, 115 bytes de tabla de archivo y numero de index de tabla de primer orden
    // fseek(memory_file, process_entry + 2 + PROCESS_NAME_SIZE + FILE_TABLE_SIZE + (first_table_index * FO_TP_ENTRY_SIZE), SEEK_SET);

    // unsigned char first_table_entry;

    // fread(&first_table_entry, 2, 1, memory_file);

    // fseek(memory_file, SO_TP_OFFSET + first_table_entry * FO_TP_ENTRY_SIZE, SEEK_SET);

    // unsigned char second_table_entry;

    // fread(&second_table_entry, )
}

void os_finish_process(int process_id) {
    int process_found = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        size_t pcb_offset = PCB_START + (i * PCB_ENTRY_SIZE);
        fseek(memory_file, pcb_offset, SEEK_SET);

        unsigned char process_state;
        unsigned char current_process_id;

        fread(&process_state, 1, 1, memory_file);
        fread(&current_process_id, 1, 1, memory_file);

        if (current_process_id == process_id && process_state == 0x01)
        {
            printf("Proceso %d encontrado.\n", current_process_id);

            fseek(memory_file, pcb_offset, SEEK_SET);

            // uint8_t cleanup_byte[1] = {0};
            // fwrite(cleanup_byte, 1, 2, memory_file);
            // fseek(memory_file, pcb_offset + 2, SEEK_SET);

            // uint8_t cleanup_name[PROCESS_NAME_SIZE] = {0};
            // fwrite(cleanup_name, 1, FILE_TABLE_SIZE, memory_file);

            fseek(memory_file, 2 + PROCESS_NAME_SIZE, SEEK_CUR);
            for (int j = 0; j < MAX_FILES; j++)
            {
                size_t file_offset = pcb_offset + 2 + PROCESS_NAME_SIZE + (j * FILE_ENTRY_SIZE); 
                fseek(memory_file, file_offset, SEEK_SET);
                unsigned char valid_byte;
                fread(&valid_byte, 1, 1, memory_file);
                if (valid_byte == 0x01)
                {
                    char file_name[14];
                    fread(file_name, FILE_NAME_SIZE, 1, memory_file);
                    unsigned int file_size;
                    fread(&file_size, FILE_SIZE, 1, memory_file);
                    unsigned char virtual_address;
                    fread(&virtual_address, FILE_VPN_SIZE, 1, memory_file);
                    free_file(pcb_offset, file_size, virtual_address);
                }
            }
            

            process_found = 1;
            // break;
        }
    }
    
    if (!process_found) {
        printf("No se ha encontrado un proceso activo con ese id\n");
    }
}