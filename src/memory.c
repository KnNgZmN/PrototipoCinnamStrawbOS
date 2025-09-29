#include <stdio.h>      // Para printf (mostrar mapa de memoria)
#include <string.h>     // (No se usa directamente aquí, pero puede quedar para extensiones)
#include "memory.h"     // Cabecera que define MemBlock, MEM_SIZE, MAX_BLOCKS, etc.
#include "log.h"       // Módulo de logging

// ======================================================
// 📌 Variables globales
// ======================================================
static MemBlock blocks[MAX_BLOCKS];  // Arreglo que representa los bloques de memoria
static int block_count = 0;          // Cantidad actual de bloques en uso

// ======================================================
// 📌 mem_init()
// Inicializa la memoria: un solo bloque libre de tamaño MEM_SIZE
// ======================================================
void mem_init() {
    block_count = 1;           // Empezamos con un solo bloque
    blocks[0].owner = -1;      // Ningún proceso dueño (sin asignar)
    blocks[0].start = 0;       // Comienza en dirección 0
    blocks[0].size = MEM_SIZE; // Tamaño completo de la memoria
    blocks[0].free = 1;        // Está libre
}

// ======================================================
// 📌 mem_coalesce()
// Función auxiliar: fusiona bloques libres adyacentes
// para evitar fragmentación externa
// ======================================================
// Fusiona bloques de memoria libres adyacentes para reducir fragmentación externa
static void mem_coalesce() {
    int i = 0;
    while (i < block_count - 1) {
        if (blocks[i].free && blocks[i+1].free) {  // Dos bloques libres consecutivos
            // Fusionar bloques contiguos
            blocks[i].size += blocks[i+1].size;

            // Desplazar los bloques a la izquierda
            for (int j = i+1; j < block_count-1; ++j) {
                blocks[j] = blocks[j+1];
            }

            block_count--; // Reducimos el total de bloques
            // No incrementamos i, revisamos de nuevo por si hay más fusiones
        } else {
            i++;
        }
    }
}

// ======================================================
// 📌 mem_alloc(owner, size)
// Implementa "first-fit": busca el primer bloque libre
// suficientemente grande y asigna la memoria.
// Devuelve el índice del bloque asignado o -1 si falla.
// ======================================================
// Asigna un bloque de memoria usando el algoritmo First-Fit
// Busca el primer bloque libre suficientemente grande
int mem_alloc(int owner, int size) {
    if (size <= 0 || size > MEM_SIZE) return -1;  // Validar tamaño

    for (int i = 0; i < block_count; ++i) {
        if (blocks[i].free && blocks[i].size >= size) {
            // Caso 1: tamaño exacto → asignar directamente
            if (blocks[i].size == size) {
                blocks[i].owner = owner;
                blocks[i].free = 0;
                return i;
            }
            // Caso 2: dividir bloque en dos (split)
            else {
                if (block_count >= MAX_BLOCKS) return -1; // No hay espacio en tabla

                // Desplazar bloques hacia la derecha para insertar el nuevo
                for (int j = block_count; j > i+1; --j) {
                    blocks[j] = blocks[j-1];
                }

                // Configurar el nuevo bloque (cola)
                blocks[i+1].owner = -1;
                blocks[i+1].start = blocks[i].start + size;
                blocks[i+1].size = blocks[i].size - size;
                blocks[i+1].free = 1;

                block_count++;

                // Configurar el bloque asignado (cabeza)
                blocks[i].size = size;
                blocks[i].owner = owner;
                blocks[i].free = 0;

                return i;
            }
        }
    }
    return -1; // No se encontró ajuste adecuado
}

// ======================================================
// 📌 mem_free_by_owner(owner)
// Libera todos los bloques pertenecientes a un proceso.
// Devuelve cuántos bloques fueron liberados.
// ======================================================
int mem_free_by_owner(int owner) {
    int freed = 0;
    for (int i = 0; i < block_count; ++i) {
        if (!blocks[i].free && blocks[i].owner == owner) {
            blocks[i].free = 1;
            blocks[i].owner = -1;
            freed++;
        }
    }
    if (freed > 0) mem_coalesce(); // Reunir bloques contiguos libres
    return freed;
}

// ======================================================
// 📌 mem_map()
// Imprime el estado actual de la memoria: índice, inicio,
// tamaño, si está libre y el PID dueño.
// ======================================================
void mem_map() {
    Mostrar("Mapa de memoria (total %d bytes):\n", MEM_SIZE);
    Mostrar("Idx\tStart\tSize\tFree\tOwner\n");
    for (int i = 0; i < block_count; ++i) {
        Mostrar("%d\t%d\t%d\t%d\t%d\n",
               i,
               blocks[i].start,
               blocks[i].size,
               blocks[i].free,
               blocks[i].owner);
    }
}
