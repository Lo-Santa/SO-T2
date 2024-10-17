# Tarea 2

## Suposiciones

### os_mount
- Suponiendo que pueda existir el error humano, está función detecta cuando a habido un error al abrir un archivo, pero solo menciona el haber de un error no lo especifica.

### os_ls_processes

### os_exists
- 

### os_ls_files
- Se asume que al ingresar un id, este será un número entre 0 hasta 255.
- Es irrelevante el estado del proceso, por ello se mostraran los archivos de incluso los procesos inactivos.
- Para cualquier momento de la llamada, el id es unico para cada proceso, por ello parar ejecución una vez encontrado el proceso correcto.
- Archivos no validos no son mostrados en la respuesta (Es decir si todos los archivos son no validos, la respuesta no muestra nada).

### os_frame_bitmap
- El estado es mostrado en hexadecimal para compactar la información
- Por cada linea se muestran 16 bytes de información sobre el estado actual del bitmap

### os_tp_bitmap
- El estado es mostrado en hexadecimal para compactar la información
- Por cada linea se muestran 16 bytes de información sobre el estado actual del bitmap


### os_start_process
- Se asume que al ingresar un id, este será un número entre 0 hasta 255.