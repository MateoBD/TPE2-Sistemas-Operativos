#ifndef _SLEEP_MANAGER_H_
#define _SLEEP_MANAGER_H_

#include <stdint.h>

/**
 * @file sleep-manager.h
 * @brief Header file for sleep management in the kernel.
 */

/**
 * @brief Suspends the execution of the calling process for a specified number of seconds.
 * @note Internally uses tick-based timing with 18 Hz frequency for more precise timing.
 */
int32_t sleep(uint64_t seconds);

/**
 * @brief Wakes up all processes that are currently sleeping.
 * @note Uses tick-based timing to check if processes should be woken up.
 */
void wake_up_sleeping_processes();

#endif /* _SLEEP_MANAGER_H_ */
