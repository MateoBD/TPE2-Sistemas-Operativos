#ifndef buddy
#include "memory-manager.h"
#include <stdint.h>
#include <stddef.h>

MemoryManagerADT memory_manager;

typedef struct
{
    void * start;
    uint64_t size;
    uint8_t used;
} MemoryFragment;

struct MemoryManagerCDT
{
    void * start_of_memory;
    MemoryFragment * page_frames;
    uint32_t page_frames_dim;
    HeapState info;
};

MemoryManagerADT memory_manager_init(void * const restrict manager_memory, void * const restrict managed_memory)
{
    MemoryManagerADT new_memory_manager = (MemoryManagerADT) manager_memory;
    
    uint64_t managed_addr = (uint64_t)managed_memory;
    uint64_t aligned_addr = (managed_addr + 7) & ~7;  // Alinear a 8 bytes

    new_memory_manager->start_of_memory = (void *)aligned_addr;
    
    new_memory_manager->page_frames = (MemoryFragment*) (manager_memory + sizeof(struct MemoryManagerCDT));
    new_memory_manager->page_frames_dim = 0;
    
    new_memory_manager->page_frames[0].start = new_memory_manager->start_of_memory;

    new_memory_manager->info.total_memory = MEMORY_SIZE;
    new_memory_manager->info.used_memory = 0;
    strncpy(new_memory_manager->info.mm_type, "gnamem", 6);
    return new_memory_manager;
}

void * memory_alloc(MemoryManagerADT const restrict self, const uint64_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    // Calcular el tamaño alineado (redondear hacia arriba al múltiplo de 8 más cercano)
    uint64_t aligned_size = (size + 7) & ~7;  // Alinea a 8 bytes
    
    uint32_t i = 0;
    void * toReturn = NULL;
    
    while (toReturn == NULL)
    {
        while (i < self->page_frames_dim && self->page_frames[i].used == 1)
        {
            i++;
        }
    
        if (self->page_frames[i].start > (void *)(self->start_of_memory + MEMORY_SIZE))
        {
            return NULL;
        }

        if (i == self->page_frames_dim || self->page_frames[i].size >= aligned_size)
        {
            if (i == self->page_frames_dim)
            {
                // Asegurar que la dirección de inicio esté alineada
                uint64_t start_addr = (uint64_t)self->page_frames[i].start;
                uint64_t aligned_start = (start_addr + 7) & ~7;  // Alinear a 8 bytes
                
                self->page_frames[i].start = (void *)aligned_start;
                self->page_frames[i].size = aligned_size;
                self->page_frames[i+1].start = (void *)(aligned_start + aligned_size);
                self->page_frames_dim++;
            }
            
            uint64_t addr = (uint64_t)self->page_frames[i].start;
            uint64_t aligned_addr = (addr + 7) & ~7;
            
            if (aligned_addr != addr)
            {
                uint64_t offset = aligned_addr - addr;
                self->page_frames[i].start = (void *)aligned_addr;
                self->page_frames[i].size -= offset;
            }
            
            toReturn = self->page_frames[i].start;
            self->page_frames[i].used = 1;
            self->info.used_memory += self->page_frames[i].size;
        }
        else
        {
            i++;
        }
    }
    
    return toReturn;
}

int memory_free(MemoryManagerADT const restrict self, void * const restrict ptr)
{
    if (ptr == NULL)
    {
        return -1;
    }

    for (uint32_t i = 0; i < self->page_frames_dim; i++)
    {
        if (self->page_frames[i].start == ptr)
        {
            self->page_frames[i].used = 0;
            self->info.used_memory -= self->page_frames[i].size;
            return 0;
        }
    }
    return -1;
}

void memory_state_get(MemoryManagerADT const restrict self, HeapState * state)
{
    if (state == NULL)
    {
        return;
    }

    state->total_memory = self->info.total_memory;
    state->used_memory = self->info.used_memory;
    state->free_memory = self->info.total_memory - self->info.used_memory;

    for (size_t i = 0; i < 6; i++)
    {
        state->mm_type[i] = self->info.mm_type[i];
    }
}
#endif
