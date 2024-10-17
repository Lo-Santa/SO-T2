#pragma once
#include <stdint.h>

#define PCB_START 0            // Inicio de la tabla de PCBs en memoria
#define PCB_ENTRY_SIZE 256           // Tamaño de cada entrada en la tabla de PCBs
#define MAX_PROCESSES 32       // Máximo número de procesos
#define PROCESS_NAME_SIZE 11   // Tamaño del nombre del proceso
#define FILE_TABLE_SIZE 115    // Tamaño de la tabla de archivos
#define FO_TP_SIZE 128    // Tamaño de la tabla de páginas de primer orden
#define FO_TP_ENTRY_SIZE 2  // Cada entrada en la tabla de páginas de segundo orden es de 2 bytes

#define MAX_FILES 5            // Máximo número de archivos por proceso
#define FILE_ENTRY_SIZE 23     // Tamaño de cada entrada de archivo
#define FILE_NAME_SIZE 14      // Espacio ocupado por el nombre de cada archivo
#define FILE_SIZE 4            // Espacio ocupado para especificar el espacio del archivo
#define FILE_VPN_SIZE 4        // Espacio ocupado por la VPN del archivo
#define VPN_BITS 12         // Number of bits in the VPN

#define FRAME_BITMAP_OFFSET (8 * 1024 + 128 + 128 * 1024) //Tamaño del offset para posicionarse en el primer bit del bitmap de frames
#define FRAME_SIZE (32 * 1024) // Tamaño de cada frame dentro de la memoria
#define FRAME_BITMAP_SIZE (8 * 1024) // Tamaño del Bitmap

#define TP_BITMAP_OFFSET (8 * 1024)  // Offset del Bitmap de Tablas de Páginas
#define TP_BITMAP_SIZE 128           // Tamaño del Bitmap de Tablas de Páginas en bytes (128 bytes)

#define SO_TP_OFFSET (8 * 1024 + 128)  // Offset al Espacio de Tablas de Segundo OrdenY

void os_mount(char* memory_path);
void os_close();
void os_ls_processes();
int os_exists(int process_id, char* file_name);
void os_ls_files(int process_id);
void os_frame_bitmap();
void os_tp_bitmap();
void os_start_process(int process_id, char* process_name);
void os_finish_process(int process_id);