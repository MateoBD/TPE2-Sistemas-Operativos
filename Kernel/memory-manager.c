#include <memory-manager.h>
#include <stdint.h>

#define NULL ((void*)0)

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
};

MemoryManagerADT newMemoryManagerADT(void * const restrict manager_memory, void * const restrict managed_memory)
{
    MemoryManagerADT new_memory_manager = (MemoryManagerADT) manager_memory;
    new_memory_manager->start_of_memory = managed_memory;
    new_memory_manager->page_frames = (MemoryFragment*) (manager_memory + sizeof(MemoryManagerADT));
    new_memory_manager->page_frames_dim = 0;
    new_memory_manager->page_frames[0].start = new_memory_manager->start_of_memory;
    return new_memory_manager;
}

void * allocMemory(MemoryManagerADT const restrict self, const uint64_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    void * toReturn = NULL;
    while (toReturn == NULL)
    {
        uint32_t i = 0;
        while ( i < self->page_frames_dim && self->page_frames[i].used == 1)
        {
            i++;
        }
    
        if ( i == self->page_frames_dim || self->page_frames[i].size >= size)
        {
            if (i == self->page_frames_dim)
            {
                self->page_frames[i].size = size;
                self->page_frames[i+1].start = self->page_frames[i].start + size;
            }
            toReturn = self->page_frames[i].start;
            self->page_frames[i].used = 1;
        }
    }
    return toReturn;
}

int freeMemory(MemoryManagerADT const restrict self, void * const restrict ptr)
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
            return 0;
        }
    }
    return -1;
}

MemoryState getStateMemory(MemoryManagerADT const restrict self)
{
    MemoryState state;
    state.total_mem = 0;
    state.used_mem = 0;
    state.free_mem = 0;

    for (uint32_t i = 0; i < self->page_frames_dim; i++)
    {
        state.total_mem += self->page_frames[i].size;
        if (self->page_frames[i].used == 1)
        {
            state.used_mem += self->page_frames[i].size;
        }
        else
        {
            state.free_mem += self->page_frames[i].size;
        }
    }

    return state;
}

