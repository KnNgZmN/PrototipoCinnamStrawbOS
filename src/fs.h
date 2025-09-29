#ifndef FS_H
#define FS_H

// =====================================================
// 📌 Definiciones para el Sistema de Archivos Virtual (VFS)
// =====================================================

// Número máximo de archivos que se pueden crear en el VFS
#define MAX_FILES 64      

// Longitud máxima permitida para el nombre de un archivo
#define MAX_NAME 64       

// Tamaño máximo del contenido de un archivo (en caracteres/bytes)
#define MAX_CONTENT 2048  

// =====================================================
// 📌 Prototipos de funciones del sistema de archivos
// =====================================================

// Busca un archivo por nombre. 
// Devuelve índice si existe, -1 si no lo encuentra.
int fs_find(const char *name);

// Inicializa la tabla de archivos (borra/limpia estado).
int fs_init();

// Crea un nuevo archivo con el nombre especificado.
// Devuelve índice del archivo o -1 si ya existe o no hay espacio.
int fs_mkfile(const char *name);

// Escribe contenido en un archivo existente.
// Si no existe, devuelve -1.
int fs_write(const char *name, const char *content);

// Lee el contenido de un archivo en el buffer 'outbuf'.
// Máximo 'maxlen' caracteres. Devuelve 0 en éxito, -1 si no existe.
int fs_read(const char *name, char *outbuf, int maxlen);

// Lista todos los archivos almacenados en el VFS.
void fs_ls();

// Guarda todo el sistema de archivos en un archivo físico en disco.
// Útil para persistencia entre ejecuciones.
int fs_save(const char *path);

// Carga el sistema de archivos desde disco (archivo persistente).
int fs_load(const char *path);

// Elimina un archivo del VFS por su nombre.
// Devuelve 0 si se eliminó, -1 si no existe.
int fs_rmfile(const char *name);

#endif // FS_H

