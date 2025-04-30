#include <gnalib.h>
#include <stdint.h>
#include <libasm.h>

uint64_t get_ticks()
{
    uint64_t ticks;
    // syscall
    return ticks;
}

#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR (60 * SECONDS_PER_MINUTE)
#define SECONDS_PER_DAY (24 * SECONDS_PER_HOUR)
#define DAYS_PER_YEAR 365

#define IS_LEAP_YEAR(year) \
    (((year) % 4 == 0 && (year) % 100 != 0) || ((year) % 400 == 0))

uint64_t time()
{
    time_t t = get_time();

    const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    uint64_t total_years = t.year + 2000 - 1970;

    uint64_t days = total_years * DAYS_PER_YEAR;

    uint64_t leap_days = 0;
    for (int y = 1970; y < t.year + 2000; y++)
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
        
        if (m == 2 &&
                ((t.year + 2000) % 4 == 0 && (t.year + 2000) % 100 != 0) ||
            ((t.year + 2000) % 400 == 0))
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
    time_t t;
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

void sleep(uint64_t seconds)
{
    // syscall
    return;
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