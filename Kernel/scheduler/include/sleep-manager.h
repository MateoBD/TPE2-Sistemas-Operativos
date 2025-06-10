#ifndef _SLEEP_MANAGER_H_
#define _SLEEP_MANAGER_H_

#include <stdint.h>
#include <processes.h>

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

/**
 * @brief Checks if a sleeping process's time has expired.
 * @param pid Process ID to check
 * @return 1 if the sleep time has expired, 0 if still sleeping, -1 if not found
 */
int32_t is_sleeping_time_expired(pid_t pid);

#endif /* _SLEEP_MANAGER_H_ */
