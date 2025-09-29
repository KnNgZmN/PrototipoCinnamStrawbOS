#ifndef MEMORY_H
#define MEMORY_H

// =====================================================
// 📌 Definiciones para el Administrador de Memoria
// =====================================================

// Tamaño total de la memoria simulada (en bytes)
#define MEM_SIZE 4096   

// Número máximo de bloques de memoria que pueden gestionarse
#define MAX_BLOCKS 64   

// =====================================================
// 📌 Estructura de un bloque de memoria
// =====================================================
typedef struct {
    int owner;   // ID del proceso propietario del bloque (-1 si está libre)
    int start;   // Dirección inicial (offset en la memoria simulada)
    int size;    // Tamaño del bloque en bytes
    int free;    // Estado del bloque: 1 = libre, 0 = ocupado
} MemBlock;

// =====================================================
// 📌 Funciones de gestión de memoria
// =====================================================

// Inicializa la memoria con un único bloque libre (MEM_SIZE completo)
void mem_init();

// Asigna memoria usando estrategia First-Fit.
// Devuelve el índice del bloque asignado o -1 si falla.
int mem_alloc(int owner, int size);

// Libera todos los bloques pertenecientes al proceso (owner).
// Devuelve la cantidad de bloques liberados.
int mem_free_by_owner(int owner);

// Imprime un mapa detallado del estado de la memoria (bloques).
void mem_map();

#endif // MEMORY_H
