#ifndef buddy
#include <heap.h>
#include <stdint.h>
#include <video-driver.h>
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
    HeapState state;
};

MemoryManagerADT memory_manager_init(void * const restrict manager_memory, void * const restrict managed_memory)
{
    vd_print("Our mm\n");
    MemoryManagerADT new_memory_manager = (MemoryManagerADT) manager_memory;
    new_memory_manager->start_of_memory = managed_memory;
    new_memory_manager->page_frames = (MemoryFragment*) (manager_memory + sizeof(struct MemoryManagerCDT));
    new_memory_manager->page_frames_dim = 0;
    new_memory_manager->page_frames[0].start = new_memory_manager->start_of_memory;
    new_memory_manager->state = (HeapState) {0, 0, 0};
    return new_memory_manager;
}

void * memory_alloc(MemoryManagerADT const restrict self, const uint64_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    uint32_t i = 0;

    void * toReturn = NULL;
    while (toReturn == NULL)
    {
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
                self->page_frames_dim++;
                self->state.total_memory += self->page_frames[i].size;
            }
            toReturn = self->page_frames[i].start;
            self->page_frames[i].used = 1;
            self->state.used_memory += self->page_frames[i].size;
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
            self->state.used_memory -= self->page_frames[i].size;
            self->state.free_memory = self->state.total_memory - self->state.used_memory;
            return 0;
        }
    }
    return -1;
}

HeapState memory_state_get(MemoryManagerADT const restrict self)
{
    return self->state;
}
#endif
