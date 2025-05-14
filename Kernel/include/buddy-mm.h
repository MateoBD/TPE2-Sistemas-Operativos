#ifndef BUDDY_MM_H
#define BUDDY_MM_H
#include <memory-manager.h>

#define TOTAL_MEMORY_TO_MANAGE_ORDER (20) //2^20 =  1 MIB
#define MIN_MEMORY_TO_MANAGE_ORDER (8) //2^8 = 256 bytes 
#define TOTAL_MEMORY_TO_MANAGE_SIZE (1 << TOTAL_MEMORY_TO_MANAGE_ORDER) //1 MiB
#define MIN_MEMORY_TO_MANAGE_SIZE (1 << MIN_MEMORY_TO_MANAGE_ORDER) //256 bytes
#define BIT_MAP_SIZE ((TOTAL_MEMORY_TO_MANAGE_SIZE/MIN_MEMORY_TO_MANAGE_SIZE)/64) // (1048576/256)/64 = 4096/64 = 64
#define MAX_LEVELS (TOTAL_MEMORY_TO_MANAGE_ORDER - MIN_MEMORY_TO_MANAGE_ORDER) // 12 levels

#define BLOCKS_AT_LEVEL(l) (1 << (MAX_LEVELS - (l))) // Número de bloques por nivel
#define BIT_WORDS_FOR_LEVEL(l) ((BLOCKS_AT_LEVEL(l) + 63) / 64) // Número de palabras (uint64_t) necesarias por nivel

#define TOTAL_BITMAP_WORDS 193  // For 1MiB of memory, we need 193 words of 64 bits each

struct MemoryManagerCDT { 
    uint64_t bitmap_mem[TOTAL_BITMAP_WORDS];
    uint64_t * bit_map[MAX_LEVELS + 1];
    void *start_of_memory;
};
void * buddy_malloc(MemoryManagerADT const restrict self, const uint64_t size);
void buddy_free(MemoryManagerADT const restrict self, void * const restrict ptr);
MemoryManagerADT buddy_init(void * const restrict manager_memory, void * const restrict managed_memory);
#endif 