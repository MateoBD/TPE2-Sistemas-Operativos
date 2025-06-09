#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>

typedef struct {
    char day;
    char month;
    char year;
    char hour;
    char min;
    char sec;
} time_struct_t;

/**
 * @brief Timer interrupt handler.
 *
 * This function is called on each timer interrupt.
 */
void timer_handler();

unsigned long get_ticks();

/**
 * @brief Retrieves the number of ticks elapsed since the system started.
 *
 * This function returns the number of timer ticks that have elapsed since the system was started.
 *
 * @return The number of ticks elapsed.
 */
int ticks_elapsed();

/**
 * @brief Retrieves the number of seconds elapsed since the system started.
 *
 * This function returns the number of seconds that have elapsed since the system was started.
 *
 * @return The number of seconds elapsed.
 */
uint64_t seconds_elapsed();

/**
 * @brief Retrieves the current system time.
 *
 * This function returns the current system time as a time structure.
 *
 * @return A time structure containing the current system time.
 */
time_struct_t get_time();
// char * getTime();
#endif
