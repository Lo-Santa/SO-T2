# Tarea 2

## Suposiciones

# Recuerda borrar la función main en osrms_File.c es solo para pruebas
### os_ls_files
- Es irrelevante el estado del proceso, por ello se mostraran los archivos de incluso los procesos inactivos.
- Para cualquier momento de la llamada, el id es unico para cada proceso, por ello parar ejecución una vez encontrado el proceso correcto.
- Archivos no validos no son mostrados en la respuesta (Es decir si todos los archivos son no validos, la respuesta no muestra nada).
#### Por el momento esta hardcodeado el uso del archivo de memoria, tiene que ser cambiado a como corresponde una vez este hecho os_mount

### os_frame_bitmap
- El estado es mostrado en hexadecimal para compactar la información
- Por cada linea se muestran 16 bytes de información sobre el estado actual del bitmap
#### Por el momento esta hardcodeado el uso del archivo de memoria, tiene que ser cambiado a como corresponde una vez este hecho os_mount

### os_tp_bitmap
- El estado es mostrado en hexadecimal para compactar la información
- Por cada linea se muestran 16 bytes de información sobre el estado actual del bitmap
#### Por el momento esta hardcodeado el uso del archivo de memoria, tiene que ser cambiado a como corresponde una vez este hecho os_mount
