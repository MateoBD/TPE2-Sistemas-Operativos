#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>

typedef struct time{
    char day;
    char month;
    char year;
    char hour;
    char min;
    char sec;
}time;

/**
 * @brief Timer interrupt handler.
 *
 * This function is called on each timer interrupt.
 */
void timer_handler();

unsigned long getTicks();

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
int seconds_elapsed();

/**
 * @brief Suspends the execution of the calling thread for a specified duration.
 *
 * This function causes the calling thread to sleep for the specified number of seconds.
 * During this time, the thread will not execute any instructions.
 *
 * @param seconds The number of seconds for which the thread should sleep.
 */

void sleep(int ticks);

/**
 * @brief Retrieves the current system time.
 *
 * This function returns the current system time as a time structure.
 *
 * @return A time structure containing the current system time.
 */
time * getTime();
// char * getTime();
#endif
