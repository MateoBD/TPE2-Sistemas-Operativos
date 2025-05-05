#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdint.h>

typedef struct
{
    uint64_t total_mem;
    uint64_t used_mem;
    uint64_t free_mem;
} MemoryState;

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT newMemoryManagerADT(void * const restrict manager_memory, void * const restrict managed_memory);

void * allocMemory(MemoryManagerADT const restrict self, const uint64_t size);

int freeMemory(MemoryManagerADT const restrict self, void * const restrict ptr);

MemoryState getStateMemory(MemoryManagerADT const restrict self);

#endif
