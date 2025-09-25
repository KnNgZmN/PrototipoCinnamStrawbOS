#include <stdio.h>     // Librería estándar de E/S (fopen, fread, fwrite, printf...)
#include <string.h>    // Manejo de cadenas (strcmp, strncpy, strlen, memset...)
#include <stdlib.h>    // Funciones estándar (malloc, free, atoi...)
#include "fs.h"        // Header propio con definiciones de constantes y prototipos

// ===============================
// Definición de estructuras
// ===============================

// Representa un archivo dentro del Sistema de Archivos Virtual (VFS)
typedef struct {
    char name[MAX_NAME];      // Nombre del archivo
    char content[MAX_CONTENT]; // Contenido almacenado en el archivo
    int used;                 // Indicador: 1 si está en uso, 0 si está vacío
} FileEntry;

// Arreglo estático de archivos (capacidad máxima definida en fs.h con MAX_FILES)
static FileEntry files[MAX_FILES];

// ===============================
// Funciones principales del VFS
// ===============================

// Inicializa el sistema de archivos virtual, marcando todas las entradas como libres
int fs_init() {
    for (int i = 0; i < MAX_FILES; ++i) 
        files[i].used = 0; // Marca todas las posiciones como "no usadas"
    return 0;
}

// Busca un archivo por nombre y devuelve su índice en el arreglo
// Retorna -1 si no existe
int fs_find(const char *name) {
    for (int i = 0; i < MAX_FILES; ++i) {
        // Verifica si la entrada está en uso y compara el nombre
        if (files[i].used && strcmp(files[i].name, name) == 0) 
            return i;
    }
    return -1; // No encontrado
}

// Elimina un archivo por nombre del VFS
// Retorna 0 si se eliminó, -1 si no existe
int fs_rmfile(const char *name) {
    int idx = fs_find(name);
    if (idx == -1) return -1; // No existe archivo con ese nombre

    // Resetea la entrada
    files[idx].used = 0;
    files[idx].name[0] = '\0';
    files[idx].content[0] = '\0';
    return 0; // Eliminado con éxito
}

// Crea un archivo vacío en el VFS
// Retorna el índice de la entrada creada o -1 si ya existe o no hay espacio
int fs_mkfile(const char *name) {
    if (fs_find(name) != -1) {
        return -1; // Ya existe un archivo con ese nombre
    }
    for (int i = 0; i < MAX_FILES; ++i) {
        if (!files[i].used) { // Encuentra una posición libre
            files[i].used = 1;
            strncpy(files[i].name, name, MAX_NAME-1); // Copia el nombre
            files[i].content[0] = '\0'; // Contenido vacío
            return i;
        }
    }
    return -1; // No hay espacio disponible
}

// Escribe contenido en un archivo existente
// Retorna 0 si éxito, -1 si el archivo no existe
int fs_write(const char *name, const char *content) {
    int idx = fs_find(name);
    if (idx == -1) return -1; // Archivo no encontrado
    strncpy(files[idx].content, content, MAX_CONTENT-1);
    return 0;
}

// Lee el contenido de un archivo y lo copia en un buffer
// Retorna 0 si éxito, -1 si el archivo no existe
int fs_read(const char *name, char *outbuf, int maxlen) {
    int idx = fs_find(name);
    if (idx == -1) return -1; // No existe
    strncpy(outbuf, files[idx].content, maxlen-1);
    return 0;
}

// Lista todos los archivos almacenados en el VFS
void fs_ls() {
    printf("Archivos en VFS (Sistema de Archivos Virtual):\n");
    for (int i = 0; i < MAX_FILES; ++i) {
        if (files[i].used) {
            printf(" - %s (len=%zu)\n", files[i].name, strlen(files[i].content));
        }
    }
}

// Guarda el estado del VFS en un archivo binario en disco
// Retorna 0 si éxito, -1 si error al abrir/escribir
int fs_save(const char *path) {
    FILE *f = fopen(path, "wb"); // Abre archivo en modo binario escritura
    if (!f) return -1;

    // Cuenta cuántos archivos están en uso
    int count = 0;
    for (int i = 0; i < MAX_FILES; ++i) 
        if (files[i].used) count++;

    // Escribe cantidad de archivos en disco
    fwrite(&count, sizeof(int), 1, f);

    // Escribe cada archivo (nombre + contenido)
    for (int i = 0; i < MAX_FILES; ++i) {
        if (!files[i].used) continue;
        int name_len = (int)strlen(files[i].name);
        int content_len = (int)strlen(files[i].content);
        fwrite(&name_len, sizeof(int), 1, f);
        fwrite(files[i].name, 1, name_len, f);
        fwrite(&content_len, sizeof(int), 1, f);
        fwrite(files[i].content, 1, content_len, f);
    }

    fclose(f);
    return 0;
}

// Carga el estado del VFS desde un archivo en disco
// Retorna 0 si éxito, -1 si error
int fs_load(const char *path) {
    FILE *f = fopen(path, "rb"); // Abre en lectura binaria
    if (!f) return -1;

    // Limpia las entradas actuales
    for (int i = 0; i < MAX_FILES; ++i) 
        files[i].used = 0;

    int count = 0;
    // Si no se puede leer la cantidad, se aborta
    if (fread(&count, sizeof(int), 1, f) != 1) { 
        fclose(f); 
        return -1; 
    }

    // Restaura cada archivo leído
    for (int i = 0; i < count; ++i) {
        int name_len = 0;
        fread(&name_len, sizeof(int), 1, f);

        char name_buf[MAX_NAME];
        memset(name_buf,0,sizeof(name_buf)); // Limpia buffer
        fread(name_buf, 1, name_len, f);

        int content_len = 0;
        fread(&content_len, sizeof(int), 1, f);

        char content_buf[MAX_CONTENT];
        memset(content_buf,0,sizeof(content_buf));
        fread(content_buf, 1, content_len, f);

        // Reconstruye el archivo en memoria
        fs_mkfile(name_buf);
        fs_write(name_buf, content_buf);
    }

    fclose(f);
    return 0;
}
