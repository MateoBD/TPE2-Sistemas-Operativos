#include <gnalib.h>
#include <stdint.h>
#include <libasm.h>
#include <stdint.h>

uint64_t get_ticks()
{
    uint64_t ticks = 0;
    return ticks;
}

#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR (60 * SECONDS_PER_MINUTE)
#define SECONDS_PER_DAY (24 * SECONDS_PER_HOUR)
#define DAYS_PER_YEAR 365

#define IS_LEAP_YEAR(year) (((year) % 4 == 0 && (year) % 100 != 0) || ((year) % 400 == 0))

uint64_t time()
{
    time_t t = get_time();

    const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    uint64_t total_years = t.year - 1970;

    uint64_t days = total_years * DAYS_PER_YEAR;

    uint64_t leap_days = 0;
    for (int y = 1970; y < t.year; y++)
    {
        if (IS_LEAP_YEAR(y))
        {
            leap_days++;
        }
    }
    days += leap_days;

    for (int m = 1; m < t.month; m++)
    {
        days += days_in_month[m];

        if (m == 2 && IS_LEAP_YEAR(t.year))
        {
            days++;
        }
    }

    days += (t.day - 1);

    uint64_t seconds = days * SECONDS_PER_DAY;
    seconds += t.hour * SECONDS_PER_HOUR;
    seconds += t.min * SECONDS_PER_MINUTE;
    seconds += t.sec;

    return seconds;
}

time_t get_time()
{
    time_t t = {0};
    return t;
}

int itoa(uint64_t value, char *buffer, int base, int n)
{
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;
    do
    {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);
    *p = 0;
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

int atoi(const char* str) {
    int result = 0;
    int sign = 1;

    while (*str == ' ' || *str == '\t') {
        str++;
    }

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}


void sleep(uint64_t ticks)
{
    sys_call(SYS_SLEEP, ticks, 0, 0, 0, 0, 0);
    return;
}

static uint32_t random_next = 1;

void srand(uint32_t seed)
{
    random_next = seed;
}

uint32_t rand(void)
{
    random_next = (random_next * 1103515245 + 12345) % RAND_MAX;
    return random_next;
}

void exit(int status)
{
    sys_call(SYS_EXIT, status, 0, 0, 0, 0, 0);
}

void *memset(void *destiation, int32_t c, uint64_t length)
{
    uint8_t chr = (uint8_t)c;
    char *dst = (char *)destiation;

    while (length--)
        dst[length] = chr;

    return destiation;
}

void *my_malloc(uint64_t size)
{
    return (void *)sys_call(SYS_MMAP, size, 0, 0, 0, 0, 0);
}

void *my_calloc(uint64_t size)
{
    void *ptr = (void *)sys_call(SYS_MMAP, size, 0, 0, 0, 0, 0);
    if (ptr != NULL)
    {
        memset(ptr, 0, size);
    }
    return ptr;
}

void my_free(void *address)
{
    sys_call(SYS_MUNMAP, (uint64_t)address, 0, 0, 0, 0, 0);
}

void get_heap_state(HeapState *state)
{
    sys_call(SYS_MEM_INFO, (uint64_t)state, 0, 0, 0, 0, 0);
}
