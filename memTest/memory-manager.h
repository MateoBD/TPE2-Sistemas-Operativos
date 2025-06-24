#ifndef HEAP_INFO_ADT_H
#define HEAP_INFO_ADT_H
#include <stdint.h>

/**
 * @file memory-manager.h
 * @brief Header file for the Memory Manager module.
 *
 * This module provides a memory management interface for dynamic allocation
 * and deallocation of memory blocks in the kernel heap space.
 */



#define MEMORY_SIZE 0x1000000 // 16 MiB
#define FREE 1
#define OCCUPIED 0

typedef struct
{
    uint64_t total_memory;
    uint64_t used_memory; 
    uint64_t free_memory; 
    char mm_type[6];      
} HeapState;

typedef struct MemoryManagerCDT *MemoryManagerADT;

/**
 * @brief Initializes the memory manager with the specified memory regions.
 *
 * This function sets up the memory manager data structures and prepares
 * the managed memory region for allocation operations.
 *
 * @param memory_manager Pointer to the memory region where the manager's
 *                      internal data structures will be stored
 * @param managed_memory Pointer to the start of the memory region that
 *                      will be managed for dynamic allocation
 * @return Initialized memory manager instance, or NULL on failure
 */
MemoryManagerADT memory_manager_init(void *const restrict memory_manager, void *const restrict managed_memory);

/**
 * @brief Allocates a block of memory of the specified size.
 *
 * Attempts to allocate a contiguous block of memory of at least the
 * requested size. The returned pointer is aligned appropriately for
 * any data type.
 *
 * @param self The memory manager instance to use for allocation
 * @param size The number of bytes to allocate (must be > 0)
 * @return Pointer to the allocated memory block, or NULL if allocation fails
 */
void *memory_alloc(MemoryManagerADT const restrict self, const uint64_t size);

/**
 * @brief Frees a previously allocated memory block.
 *
 * Returns the specified memory block to the free pool, making it available
 * for future allocations. The pointer becomes invalid after this call.
 *
 * @param self The memory manager instance that allocated the memory
 * @param ptrs Pointer to the memory block to free (must have been returned
 *            by a previous call to memory_alloc with the same manager)
 * @return 0 on success, negative value on error (e.g., invalid pointer)
 */
int memory_free(MemoryManagerADT const restrict self, void *const restrict ptrs);

/**
 * @brief Retrieves the current state of the memory manager.
 *
 * Fills the provided HeapState structure with current memory usage
 * statistics and manager information.
 *
 * @param self The memory manager instance to query
 * @param state Pointer to a HeapState structure that will be filled
 *             with current memory statistics
 */
void memory_state_get(MemoryManagerADT const restrict self, HeapState *state);

/** @brief Global memory manager instance used throughout the kernel */
extern MemoryManagerADT memory_manager;
#endif