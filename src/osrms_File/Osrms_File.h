#pragma once
#include <stdint.h>

#define PCB_START 0            // Inicio de la tabla de PCBs en memoria
#define PCB_SIZE 256           // Tamaño de cada entrada en la tabla de PCBs
#define PCB_NAME_SIZE 11       // Tamaño del nombre del proceso
#define MAX_PROCESSES 32       // Máximo número de procesos
#define MAX_FILES 5            // Máximo número de archivos por proceso
#define FILE_ENTRY_SIZE 23     // Tamaño de cada entrada de archivo

#define FRAME_BITMAP_OFFSET (8 * 1024 + 128 + 128 * 1024)  // Offset al Frame Bitmap (basado en la estructura de memoria)
#define FRAME_BITMAP_SIZE 8192  // 8 KB

#define TP_BITMAP_OFFSET (8 * 1024)  // Offset del Bitmap de Tablas de Páginas (después de los primeros 8 KB)
#define TP_BITMAP_SIZE 128           // Tamaño del Bitmap de Tablas de Páginas en bytes (128 bytes)


void os_ls_files(int process_id);
void os_frame_bitmap();