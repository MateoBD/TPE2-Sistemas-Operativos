#ifndef _GNALIB_H_
#define _GNALIB_H_

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define RAND_MAX 0x7FFFFFFF

typedef struct {
    char day;
    char month;
    char year;
    char hour;
    char min;
    char sec;
} time_t;

uint64_t get_ticks();

uint64_t time();

time_t get_time();

int itoa(uint64_t value, char * buffer, int base, int n);

void sleep(uint32_t seconds);

void srand(uint32_t seed);

uint32_t rand();

#endif