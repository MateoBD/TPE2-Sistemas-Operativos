#ifndef _PIPES_H_
#define _PIPES_H_

#include <stdint.h>

/**
 * @file pipes.h
 * @brief Header file for pipe management in the kernel.
 *
 * This header provides the interface for creating, managing, and using pipes
 * for inter-process communication. Pipes support blocking read/write operations
 * using semaphores for synchronization.
 */

/**
 * @brief Initializes the pipe manager.
 *
 * This function initializes the global pipe manager, setting up the pipe table
 * and marking the first three entries (STDIN, STDOUT, STDERR) as used by default.
 * This function is called automatically when needed, but can be called explicitly.
 */
void init_pipe_manager(void);

/**
 * @brief Creates a new pipe.
 *
 * Creates a new pipe and returns its file descriptor. The pipe can be used
 * for reading and writing data between processes. The pipe uses semaphores
 * for synchronization to handle blocking operations.
 *
 * @return File descriptor of the new pipe on success, -1 on error.
 */
int create_pipe(void);

/**
 * @brief Closes a pipe.
 *
 * Closes the specified pipe, freeing its resources and cleaning up associated
 * semaphores. The pipe file descriptor becomes invalid after this call.
 *
 * @param fd File descriptor of the pipe to close.
 * @return 0 on success, -1 on error.
 */
int close_pipe(uint16_t fd);

/**
 * @brief Reads data from a pipe.
 *
 * Reads up to 'count' bytes from the specified pipe into the provided buffer.
 * This operation blocks if no data is available until data is written to the pipe.
 * Cannot read from STDOUT or STDERR pipes.
 *
 * @param fd File descriptor of the pipe to read from.
 * @param buffer Buffer to store the read data.
 * @param count Maximum number of bytes to read.
 * @return Number of bytes actually read on success, -1 on error.
 */
int read_pipe(uint16_t fd, int8_t *buffer, int count);

/**
 * @brief Writes data to a pipe.
 *
 * Writes up to 'count' bytes from the provided buffer to the specified pipe.
 * This operation blocks if the pipe buffer is full until space becomes available.
 * Cannot write to STDIN pipe.
 *
 * @param fd File descriptor of the pipe to write to.
 * @param buffer Buffer containing the data to write.
 * @param count Number of bytes to write.
 * @return Number of bytes actually written on success, -1 on error.
 */
int write_pipe(int fd, const int8_t *buffer, int count);

/**
 * @brief Reads data from a pipe without blocking.
 *
 * Attempts to read up to 'count' bytes from the specified pipe into the provided buffer.
 * This operation does not block if no data is available. Returns immediately with
 * the number of bytes actually read (may be 0 if no data available).
 * Cannot read from STDOUT or STDERR pipes.
 *
 * @param fd File descriptor of the pipe to read from.
 * @param buffer Buffer to store the read data.
 * @param count Maximum number of bytes to read.
 * @return Number of bytes actually read on success (may be 0), -1 on error.
 */
int read_pipe_nonblocking(uint16_t fd, int8_t *buffer, int count);

/**
 * @brief Writes data to a pipe without blocking.
 *
 * Attempts to write up to 'count' bytes from the provided buffer to the specified pipe.
 * This operation does not block if the pipe buffer is full. Returns immediately with
 * the number of bytes actually written (may be 0 if buffer is full).
 * Cannot write to STDIN pipe.
 *
 * @param fd File descriptor of the pipe to write to.
 * @param buffer Buffer containing the data to write.
 * @param count Number of bytes to write.
 * @return Number of bytes actually written on success (may be 0), -1 on error.
 */
int write_pipe_nonblocking(int fd, const int8_t *buffer, int count);

#endif /* _PIPES_H_ */