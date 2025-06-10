#include <time.h>
#include <times.h>
#include <libasm.h>
#include <processes.h>
#include <sleep-manager.h>

static time_struct_t start_up_time;
static int initialized = 0;
static uint64_t ticks = 0;

unsigned long get_ticks()
{
    return ticks;
}

void init_time()
{
    start_up_time = get_time();
    initialized = 1;
}

void timer_handler()
{
    if (!initialized)
    {
        init_time();
    }
    ticks++;

    wake_up_sleeping_processes();
}

static int is_leap_year(int year)
{
    return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

static int days_in_month(int month, int year)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year))
        return 29;
    return days[month - 1];
}

static uint32_t date_to_days(int day, int month, int year)
{
    uint32_t days = day - 1;

    for (int m = 1; m < month; m++)
    {
        days += days_in_month(m, year);
    }

    for (int y = 0; y < year; y++)
    {
        days += 365 + is_leap_year(y);
    }

    return days;
}

uint64_t time_to_epoch_seconds(time_struct_t t)
{
    uint32_t total_days = date_to_days(t.day, t.month, t.year);
    return (uint64_t)total_days * 86400 +
           (uint64_t)t.hour * 3600 +
           (uint64_t)t.min * 60 +
           (uint64_t)t.sec;
}

uint64_t seconds_elapsed()
{
    uint64_t t1 = time_to_epoch_seconds(start_up_time);
    uint64_t t2 = time_to_epoch_seconds(get_time());
    return (t2 > t1) ? (t2 - t1) : (t1 - t2);
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
