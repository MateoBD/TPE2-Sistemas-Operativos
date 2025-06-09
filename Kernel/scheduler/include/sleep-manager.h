#ifndef _SLEEP_MANAGER_H_
#define _SLEEP_MANAGER_H_

#include <stdint.h>

/**
 * @file sleep-manager.h
 * @brief Header file for sleep management in the kernel.
 */

/**
 * @brief Suspends the execution of the calling process for a specified number of seconds.
 */
int32_t sleep(uint64_t seconds);

/**
 * @brief Wakes up all processes that are currently sleeping.
 */
void wake_up_sleeping_processes();

#endif /* _SLEEP_MANAGER_H_ */
