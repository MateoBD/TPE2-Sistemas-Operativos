#include <time.h>
#include <times.h>
#include <libasm.h>

static unsigned long ticks = 0;

void timer_handler()
{
    ticks++;
}

int ticks_elapsed()
{
    return ticks;
}

int seconds_elapsed()
{
    return ticks / 18;
}

unsigned long get_ticks()
{
    return ticks;
}

void sleep(int ticks_to_wait)
{
    unsigned long start = ticks;
    while (ticks - start < ticks_to_wait)
    {
        _hlt();
    };
}


time_struct_t get_time()
{
    time_struct_t t;
    t.day = get_system_day_of_month();
    t.month = get_system_month();
    t.year = get_system_year();
    t.hour = get_system_hour();
    t.min = get_system_min();
    t.sec = get_system_sec();
    return t;
}
