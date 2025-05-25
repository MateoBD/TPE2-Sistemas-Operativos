#include <gnalib.h>
#include <stdint.h>
#include <libasm.h>
#include <stdint.h>

uint64_t get_ticks()
{
    uint64_t ticks = 0;
    // syscall
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
    // syscall
    return t;
}

int itoa(uint64_t value, char *buffer, int base, int n)
{
    int i = 0;
    if (value == 0)
    {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return i;
    }
    while (value > 0 && i < n)
    {
        int digit = value % base;
        buffer[i++] = (digit < 10) ? '0' + digit : 'A' + digit - 10;
        value /= base;
    }
    buffer[i] = '\0';
    return i;
}

void sleep(uint64_t ticks)
{
    sys_call(SYS_SLEEP, ticks, 0, 0, 0, 0, 0);
}

static uint32_t random_next = 1;

void srand(uint32_t seed)
{
    random_next = seed;
}

uint32_t rand()
{
    random_next = (random_next * 1103515245 + 12345) % RAND_MAX;
    return random_next;
}

uint64_t create_process(void * function, int argc, char **argv)
{
    sys_call((uint64_t)SYS_CREATE, (uint64_t) function, (uint64_t) argc, (uint64_t) argv, 0, 0, 0);
    return 0;
}