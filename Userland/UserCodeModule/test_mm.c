#include <stdint.h>
#include <gnaio.h>
#include <gnalib.h>
#include <test.h>
#define STDOUT 1

#define MAX_BLOCKS 1024

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t GetUint() {
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;
}

uint32_t GetUniform(uint32_t max) {
  uint32_t u = GetUint();
  return (u + 1.0) * 2.328306435454494e-10 * max;
}

int64_t satoi(uint8_t *str) {
  uint64_t i = 0;
  int64_t res = 0;
  int8_t sign = 1;

  if (!str)
    return 0;

  if (str[i] == '-') {
    i++;
    sign = -1;
  }

  for (; str[i] != '\0'; ++i) {
    if (str[i] < '0' || str[i] > '9')
      return 0;
    res = res * 10 + str[i] - '0';
  }

  return res * sign;
}

uint8_t memcheck(void *start, uint8_t value, uint32_t size) {
  uint8_t *p = (uint8_t *)start;
  uint32_t i;

  for (i = 0; i < size; i++, p++)
    if (*p != value)
      return 0;

  return 1;
}

int64_t test_mm() {
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

  HeapState state;
  get_heap_state(&state);
  state.free_memory=1;

  while (1){
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (state.free_memory!=0) {
      mm_rqs[rq].size = GetUniform(state.total_memory - total - 1) + 1;
      mm_rqs[rq].address = my_malloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        get_heap_state(&state);
        set_color(magenta, black);
        printf("allocating at least %d bytes at %x\n", mm_rqs[rq].size, mm_rqs[rq].address);
        print_memory_state();
        sleep(64);
        clean_screen();
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
            set_color(red, black);
            printf("ERROR in test_mm: memcheck failed\n");
            return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address){
        get_heap_state(&state);
        set_color(green, black);
        printf("freeing at least %d bytes at %x\n", mm_rqs[i].size, mm_rqs[i].address);
        print_memory_state();
        sleep(64);
        clean_screen();
        my_free(mm_rqs[i].address);
      }
  }
  
  return 0;
}