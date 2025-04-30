#ifndef _GNALIB_H_
#define _GNALIB_H_

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

time_t get_time();

int itoa(uint64_t value, char * buffer, int base, int n);

void sleep(uint64_t seconds);

void srand(uint64_t seed);

uint64_t rand();

#endif