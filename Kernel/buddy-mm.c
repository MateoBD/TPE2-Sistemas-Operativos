#include <stdio.h>
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


MemoryManagerADT buddy_init(void * const restrict manager_memory, void * const restrict managed_memory)
{
    MemoryManagerADT new_memory_manager = (MemoryManagerADT) manager_memory;
    new_memory_manager->start_of_memory = managed_memory;

    size_t offset=0;
    for (uint8_t level = 0; level <= MAX_LEVELS; level++) {
        size_t words = BIT_WORDS_FOR_LEVEL(level);
        new_memory_manager->bit_map[level] = &new_memory_manager->bitmap_mem[offset];
        for (size_t i = 0; i < words; i++) {
            new_memory_manager->bit_map[level][i] = 0;
        }
        offset += words;
    }

    return new_memory_manager;
}

uint64_t round_up_to_power_of_two(size_t size)
{
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size++;
    return size;
}

uint8_t getLevel(uint64_t  real_size)
{
    uint8_t level = 0;
    if (real_size == MIN_MEMORY_TO_MANAGE_SIZE)
    {
        return MAX_LEVELS;
    }
    real_size /= MIN_MEMORY_TO_MANAGE_SIZE;
    while (real_size > 1)
    {
        real_size /= 2;
        level++;
    }
    return MAX_LEVELS-level;
}

uint16_t getLevelOffset(uint8_t level) {
    uint16_t offset = 0;
    for (uint8_t i = 0; i < level; i++) {
        offset += (1 << (MAX_LEVELS - i));
    }
    return offset;
}

int getBit(uint64_t *bitmap_level, uint16_t index) {
    return (bitmap_level[index / 64] >> (index % 64)) & 1;
}

void setBit(uint64_t *bitmap_level, uint16_t index, int value) {
    if (value)
        bitmap_level[index / 64] |= (1ULL << (index % 64));
    else
        bitmap_level[index / 64] &= ~(1ULL << (index % 64));
}



void *lookup_table(MemoryManagerADT const restrict memory_manager, uint8_t level, uint64_t real_size) {
    for (uint8_t l = level; l <= MAX_LEVELS; l++) {
        uint16_t blocks = 1 << (MAX_LEVELS - l);
        for (uint16_t i = 0; i < blocks; i++) {
            if (!getBit(memory_manager->bit_map[l], i)) {
                // Encontramos un bloque libre en un nivel superior, dividir
                // Marcar como ocupado
                setBit(memory_manager->bit_map[l], i, 1);

                // Split hacia abajo hasta el nivel deseado
                while (l > level) {
                    l--;
                    i *= 2;

                    // Mitad derecha queda libre
                    setBit(memory_manager->bit_map[l], i + 1, 0);
                    // Mitad izquierda queda ocupada y seguimos dividiendo
                    setBit(memory_manager->bit_map[l], i, 1);
                }

                // Calcular puntero al bloque
                size_t block_size = MIN_MEMORY_TO_MANAGE_SIZE << (MAX_LEVELS - level);
                void *ptr = (void *)((char *)memory_manager->start_of_memory + i * block_size);

                // Guardar nivel al que pertenece el bloque
                ((uint8_t *)ptr)[0] = level;
                return (void *)((uint8_t *)ptr + 1);
            }
        }
    }

    return NULL; // No se encontró memoria disponible
}



void * buddy_malloc(MemoryManagerADT const restrict self, const uint64_t size)
{
    if (size == 0)
    {
        return NULL;
    }
    uint64_t real_size = round_up_to_power_of_two(size+sizeof(uint8_t));
    uint8_t level = getLevel(real_size);
    void * memory = lookup_table(self,level,real_size);
    if (memory == NULL)
    {
        return NULL;
    }
    // Guardamos el tamaño real en el primer byte
    ((uint8_t *)memory)[0] = level;
    return (void *)((uint8_t *)memory + 1);
}

void buddy_free(MemoryManagerADT const restrict self, void * const restrict ptr)
{
    if (ptr == NULL) return;

    uint8_t *real_ptr = (uint8_t *)ptr - 1;
    uint8_t level = real_ptr[0];

    size_t block_size = MIN_MEMORY_TO_MANAGE_SIZE << (MAX_LEVELS - level);
    size_t offset = (size_t)(real_ptr - (uint8_t *)self->start_of_memory);
    uint16_t index = offset / block_size;

    while (level <= MAX_LEVELS) {
        // Liberar bloque actual
        setBit(self->bit_map[level], index, 0);

        // Calcular índice del buddy
        uint16_t buddy_index = index ^ 1;

        // Si el buddy no está libre, detenemos el merge
        if (getBit(self->bit_map[level], buddy_index)) {
            break;
        }

        // Si el buddy también está libre, lo liberamos
        setBit(self->bit_map[level], buddy_index, 0);

        // Subimos de nivel y recalculamos índice del padre
        index /= 2;
        level++;
    }
}