#include <stdio.h>
#include <string.h>
#include "memory.h"

static MemBlock blocks[MAX_BLOCKS];
static int block_count = 0;

void mem_init() {
    block_count = 1;
    blocks[0].owner = -1;
    blocks[0].start = 0;
    blocks[0].size = MEM_SIZE;
    blocks[0].free = 1;
}

// helper: merge adjacent free blocks
static void mem_coalesce() {
    int i = 0;
    while (i < block_count - 1) {
        if (blocks[i].free && blocks[i+1].free) {
            blocks[i].size += blocks[i+1].size;
            // shift left
            for (int j = i+1; j < block_count-1; ++j) blocks[j] = blocks[j+1];
            block_count--;
            // don't increment i, check again
        } else i++;
    }
}

// first-fit allocation, returns block index or -1
int mem_alloc(int owner, int size) {
    if (size <= 0 || size > MEM_SIZE) return -1;
    for (int i = 0; i < block_count; ++i) {
        if (blocks[i].free && blocks[i].size >= size) {
            if (blocks[i].size == size) {
                blocks[i].owner = owner;
                blocks[i].free = 0;
                return i;
            } else {
                // split block
                if (block_count >= MAX_BLOCKS) return -1;
                // new block for the tail
                for (int j = block_count; j > i+1; --j) blocks[j] = blocks[j-1];
                blocks[i+1].owner = -1;
                blocks[i+1].start = blocks[i].start + size;
                blocks[i+1].size = blocks[i].size - size;
                blocks[i+1].free = 1;
                block_count++;
                // set current block
                blocks[i].size = size;
                blocks[i].owner = owner;
                blocks[i].free = 0;
                return i;
            }
        }
    }
    return -1; // no fit
}

// free all blocks for owner; return number freed
int mem_free_by_owner(int owner) {
    int freed = 0;
    for (int i = 0; i < block_count; ++i) {
        if (!blocks[i].free && blocks[i].owner == owner) {
            blocks[i].free = 1;
            blocks[i].owner = -1;
            freed++;
        }
    }
    if (freed > 0) mem_coalesce();
    return freed;
}

void mem_map() {
    printf("Mapa de memoria (total %d bytes):\n", MEM_SIZE);
    printf("Idx\tStart\tSize\tFree\tOwner\n");
    for (int i = 0; i < block_count; ++i) {
        printf("%d\t%d\t%d\t%d\t%d\n", i, blocks[i].start, blocks[i].size, blocks[i].free, blocks[i].owner);
    }
}
