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
    
    // Calcular dirección alineada a 8 bytes
    uint64_t managed_addr = (uint64_t)managed_memory;
    uint64_t aligned_addr = (managed_addr + 7) & ~7;  // Alinear a 8 bytes

    new_memory_manager->start_of_memory = (void *)aligned_addr;
    
    // Configurar estructuras del gestor de memoria
    new_memory_manager->page_frames = (MemoryFragment*) (manager_memory + sizeof(struct MemoryManagerCDT));
    new_memory_manager->page_frames_dim = 0;
    
    // Configurar el primer page_frame con la dirección alineada
    new_memory_manager->page_frames[0].start = new_memory_manager->start_of_memory;
    
    // Inicializar estado de memoria
    new_memory_manager->state = (MemoryState) {0, 0, 0};
    
    return new_memory_manager;
}

void * alloc_memory(MemoryManagerADT const restrict self, const uint64_t size)
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
    
        if (i == self->page_frames_dim || self->page_frames[i].size >= aligned_size)
        {
            if (i == self->page_frames_dim)
            {
                // Asegurar que la dirección de inicio esté alineada
                uint64_t start_addr = (uint64_t)self->page_frames[i].start;
                uint64_t aligned_start = (start_addr + 7) & ~7;  // Alinear a 8 bytes
                
                // Ajustar por la diferencia si hubo alineación
                uint64_t alignment_offset = aligned_start - start_addr;
                
                self->page_frames[i].start = (void *)aligned_start;
                self->page_frames[i].size = aligned_size;
                self->page_frames[i+1].start = (void *)(aligned_start + aligned_size);
                self->page_frames_dim++;
                self->state.total_mem += self->page_frames[i].size + alignment_offset;
            }
            
            // Asegurar que la dirección devuelta esté alineada
            uint64_t addr = (uint64_t)self->page_frames[i].start;
            uint64_t aligned_addr = (addr + 7) & ~7;
            
            if (aligned_addr != addr) {
                // Si necesitamos alinear, ajustar el fragmento
                uint64_t offset = aligned_addr - addr;
                self->page_frames[i].start = (void *)aligned_addr;
                self->page_frames[i].size -= offset;
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

