#ifndef MEMORY_H
#define MEMORY_H

#define MEM_SIZE 4096   // memoria simulada (bytes)
#define MAX_BLOCKS 64

typedef struct {
    int owner;   // id del proceso propietario, -1 si libre
    int start;   // offset inicial
    int size;    // tamaño en bytes
    int free;    // 1 = libre, 0 = ocupado
} MemBlock;

void mem_init();
int mem_alloc(int owner, int size); // retorna índice de bloque o -1
int mem_free_by_owner(int owner); // libera bloques pertenecientes a owner; retorna cantidad liberada
void mem_map(); // imprime mapa de memoria
#endif
