#ifdef buddy
#include "memory-manager.h"
#include <stddef.h>

#define MIN_LEVEL 5
#define MAX_ORDER 25

MemoryManagerADT memory_manager;

typedef struct block_t {
  int8_t order;
  int8_t status;
  struct block_t *next;
} block_t;

typedef struct MemoryManagerCDT {
  int8_t max_order;
  block_t *free_blocks[MAX_ORDER];
  HeapState info;
  void *start_of_memory;
} buddy_manager;

static block_t *create_block(MemoryManagerADT const restrict self, void *address, int8_t order) {
  block_t *block = (block_t *)address;
  block->order = order;
  block->status = FREE;
  block->next = self->free_blocks[order];
  self->free_blocks[order] = block;
  return block;
}

static void remove_block(MemoryManagerADT const restrict self, block_t *block) {
  if (block == NULL) {
    return;
  }
  uint8_t order = block->order;
  block_t *curr_block = self->free_blocks[order];
  if (curr_block == NULL) {
    return;
  }

  if (curr_block == block) {
    self->free_blocks[order]->status = OCCUPIED;
    self->free_blocks[order] = self->free_blocks[order]->next;
    return;
  }

  while (curr_block->next != NULL && curr_block->next != block) {
    curr_block = curr_block->next;
  }

  if (curr_block->next == NULL) {
    return;
  }

  curr_block->next = curr_block->next->next;
  curr_block->status = OCCUPIED;

  return;
}

static void split_block(MemoryManagerADT const restrict self, int8_t order) {
  block_t *block = self->free_blocks[order];
  remove_block(self, block);
  block_t *buddy_block = (block_t *)((void *)block + (1L << (order - 1)));
  create_block(self, (void *)buddy_block, order - 1);
  create_block(self, (void *)block, order - 1);

  return;
}

static block_t *merge(MemoryManagerADT const restrict self, block_t *block, block_t *buddy_block) {
  if (block == NULL || buddy_block == NULL) {
    return NULL;
  }

  block_t *left = block < buddy_block ? block : buddy_block;
  remove_block(self, buddy_block);
  left->order++;
  left->status = FREE;
  return left;
}

MemoryManagerADT memory_manager_init(void * const restrict manager_memory, void * const restrict managed_memory) {
  MemoryManagerADT mm = (MemoryManagerADT)manager_memory;
  int current_size = 2;
  int level = 1;
  while (current_size < MEMORY_SIZE) {
    level++;
    current_size *= 2;
  }
  mm->max_order = level;
  mm->info.total_memory = MEMORY_SIZE;
  mm->info.used_memory = 0;
  mm->start_of_memory = managed_memory;
  strncpy(mm->info.mm_type, "buddy", 6);
  
  for (int i = 0; i < MAX_ORDER; i++) {
    mm->free_blocks[i] = NULL;
  }
  create_block(mm, (void *) managed_memory, level);
  return mm;
}

void * memory_alloc(MemoryManagerADT const restrict self, const uint64_t size) {
  if (size <= 0 || size > MEMORY_SIZE) {
    return NULL;
  }

  int8_t order = 1;
  int64_t block_size = 2;
  while (block_size < size + sizeof(block_t)) {
    order++;
    block_size *= 2;
  }

  order = (MIN_LEVEL > order) ? MIN_LEVEL : order;

  if (self->free_blocks[order] == NULL) {
    uint8_t order_approx = 0;
    for (uint8_t i = order + 1; i <= self->max_order && order_approx == 0;
         i++) {
      if (self->free_blocks[i] != NULL) {
        order_approx = i;
      }
    }

    if (order_approx == 0) {
      return NULL;
    }

    while (order_approx > order) {
      split_block(self, order_approx);
      order_approx--;
    }
  }

  block_t *block = self->free_blocks[order];
  remove_block(self, block);
  block->status = OCCUPIED;

  return (void *)((uint8_t *)block + sizeof(block_t));
}

int memory_free(MemoryManagerADT const restrict self, void * const restrict ptrs) {
  if (ptrs == NULL) {
    return -1;
  }

  block_t *block = (block_t *)(ptrs - sizeof(block_t));
  if (block == NULL || block->status == FREE) {
    return -1;
  }
  block->status = FREE;

  uint64_t block_pos = (uint64_t)((void *)block - self->start_of_memory);
  block_t *buddy_block = (block_t *)((uint64_t)self->start_of_memory + (((uint64_t)block_pos) ^ ((1L << block->order))));
  if (buddy_block == NULL) {
    return -1;
  }
  while (block && buddy_block && block->order < self->max_order && buddy_block->status == FREE && buddy_block->order == block->order) {
    block = merge(self, block, buddy_block);
    if (block == NULL) { // excessive checking to avoid PVS warning
      return -1;
    }
    block->status = FREE;
    block_pos = (uint64_t)((void *)block - ((uint64_t)self->start_of_memory));
    buddy_block = (block_t *)(block_t *)((uint64_t)self->start_of_memory + (((uint64_t)block_pos) ^ ((1L << block->order))));
  }

  if (block == NULL) {
    return -1;
  }
  create_block(self, (void *)block, block->order);

  return 0;
}

void memory_state_get(MemoryManagerADT const restrict self, HeapState * state) {
  self->info.free_memory = 0;
  for (uint64_t i = MIN_LEVEL; i < self->max_order; i++) {
    block_t *current_block = self->free_blocks[i];
    while (current_block != NULL) {
      self->info.free_memory += (uint64_t)((uint64_t)1 << i);
      current_block = current_block->next;
    }
  }
  state->total_memory = self->info.total_memory;
  state->used_memory = self->info.total_memory - self->info.free_memory;
  state->free_memory = self->info.free_memory;
  for (size_t i = 0; i < 6; i++)
  {
    state->mm_type[i] = self->info.mm_type[i];
  }
}
#endif