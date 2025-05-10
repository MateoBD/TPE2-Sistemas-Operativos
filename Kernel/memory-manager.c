#include <memory-manager.h>
#include <stdint.h>
#include <video-driver.h>

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
    MemoryState state;
};

MemoryManagerADT new_memory_managerADT(void * const restrict manager_memory, void * const restrict managed_memory)
{
    MemoryManagerADT new_memory_manager = (MemoryManagerADT) manager_memory;
    new_memory_manager->start_of_memory = managed_memory;
    new_memory_manager->page_frames = (MemoryFragment*) (manager_memory + sizeof(struct MemoryManagerCDT));
    new_memory_manager->page_frames_dim = 0;
    new_memory_manager->page_frames[0].start = new_memory_manager->start_of_memory;
    new_memory_manager->state = (MemoryState) {0, 0, 0};
    return new_memory_manager;
}

void * alloc_memory(MemoryManagerADT const restrict self, const uint64_t size)
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
                self->state.total_mem += self->page_frames[i].size;
            }
            toReturn = self->page_frames[i].start;
            self->page_frames[i].used = 1;
            self->state.used_mem += self->page_frames[i].size;
        }
        else
        {
            i++;
        }
    }
    return toReturn;
}

int free_memory(MemoryManagerADT const restrict self, void * const restrict ptr)
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
            self->state.used_mem -= self->page_frames[i].size;
            self->state.free_mem = self->state.total_mem - self->state.used_mem;
            return 0;
        }
    }
    return -1;
}

void print_state_memory(MemoryManagerADT const restrict self)
{
    vd_print("Page Frames Dim: ");
    vd_print_dec(self->page_frames_dim);
    vd_draw_char('\n');
    for(int i = 0; i < self->page_frames_dim; i++) {
        vd_print("Fragment: 0x");
        vd_print_hex((uint64_t)self->page_frames[i].start);
        vd_print("    ");
        vd_print("Size: 0x");
        vd_print_hex(self->page_frames[i].size);
        vd_print("    ");
        vd_print("Used: ");
        vd_print_hex(self->page_frames[i].used);
        vd_draw_char('\n');
    }
}

MemoryState get_state_memory(MemoryManagerADT const restrict self)
{
    return self->state;
}

