#ifndef HEAP_INFO_ADT_H
#define HEAP_INFO_ADT_H
#include <stdint.h>

#define MEMORY_START 0x0000000000200000
#define MEMORY_END 0x0000000000400000
#define MEMORY_SIZE (MEMORY_END - MEMORY_START)

#define FREE 1
#define OCCUPIED 0

typedef struct
{
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
} HeapState;

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT memory_manager_init (void * const restrict memory_manager, void * const restrict managed_memory);
void * memory_alloc(MemoryManagerADT const restrict self, const uint64_t size);
int memory_free(MemoryManagerADT const restrict self, void * const restrict ptrs);
HeapState memory_state_get(MemoryManagerADT const restrict self);

extern MemoryManagerADT memory_manager;
#endif