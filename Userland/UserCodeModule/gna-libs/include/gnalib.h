#ifndef _GNALIB_H_
#define _GNALIB_H_

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define RAND_MAX 0x7FFFFFFF

typedef struct
{
    char day;
    char month;
    char year;
    char hour;
    char min;
    char sec;
} time_t;

typedef struct
{
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
    char mm_type[6];
} HeapState;

uint64_t get_ticks();

uint64_t time();

time_t get_time();

int itoa(uint64_t value, char *buffer, int base, int n);

int atoi(const char *str);

void sleep(uint64_t ticks);

void srand(uint32_t seed);

uint32_t rand();

void exit(int status);

void *memset(void *destiny, int32_t c, uint64_t length);

void *my_malloc(uint64_t size);

void *my_calloc(uint64_t size);

void my_free(void *address);

void get_heap_state(HeapState *state);

#endif