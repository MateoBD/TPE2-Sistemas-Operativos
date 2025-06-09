#ifndef TEST_PROGRAMS_H
#define TEST_PROGRAMS_H

/**
 * @file test-programs.h
 * @brief Header file for test program wrapper functions.
 *
 * This file contains declarations for wrapper functions that allow
 * various test programs to be executed as independent processes
 * from the shell interface.
 */

/**
 * @brief Wrapper function for the process creation and management test.
 *
 * This function creates a process to run test_processes with the specified
 * maximum number of processes parameter.
 *
 * @param argc Number of arguments
 * @param argv Array of arguments, where argv[1] should be max_processes
 */
void test_processes_wrapper(int argc, char **argv, uint16_t * fds);

/**
 * @brief Wrapper function for the memory manager allocation/deallocation test.
 *
 * This function creates a process to run test_mm with the specified
 * maximum memory parameter.
 *
 * @param argc Number of arguments
 * @param argv Array of arguments, where argv[1] should be max_memory
 */
void test_mm_wrapper(int argc, char **argv, uint16_t * fds);

/**
 * @brief Wrapper function for the priority-based scheduling test.
 *
 * This function creates a process to run test_prio, which tests
 * priority-based process scheduling.
 *
 * @param argc Number of arguments (not used)
 * @param argv Array of arguments (not used)
 */
void test_prio_wrapper(int argc, char **argv, uint16_t * fds);

/**
 * @brief Wrapper function for the semaphore synchronization test.
 *
 * This function creates a process to run test_sync with the specified
 * parameters for testing semaphore-based synchronization.
 *
 * @param argc Number of arguments
 * @param argv Array of arguments, where:
 *             argv[1] = number of iterations
 *             argv[2] = use semaphore flag (0 or 1)
 *             argv[3] = semaphore ID string
 */
void test_sync_wrapper(int argc, char **argv, uint16_t * fds);

/**
 * @brief Wrapper function for a basic memory manager test.
 *
 * This function creates a process to run a basic memory test
 * with default parameters (1024 bytes).
 *
 * @param argc Number of arguments (not used)
 * @param argv Array of arguments (not used)
 */
void test_mm_basic_wrapper(int argc, char **argv, uint16_t * fds);

/**
 * @brief Wrapper function for displaying test help information.
 *
 * This function creates a process to display help information
 * about available test commands and their usage.
 *
 * @param argc Number of arguments (not used)
 * @param argv Array of arguments (not used)
 */
void test_help_wrapper(int argc, char **argv, uint16_t * fds);

/**
 * @brief Unified test command function.
 *
 * This function handles all test commands through a single interface.
 * It parses the test type and delegates to the appropriate wrapper function.
 *
 * @param argc Number of arguments
 * @param argv Array of arguments, where argv[1] is the test type
 */
void test_command(int argc, char **argv, uint16_t * fds);

#endif // TEST_PROGRAMS_H