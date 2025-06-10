#ifndef _LIBASM_H_
#define _LIBASM_H_

#include <stdint.h>

enum enum_syscalls
{
SYS_WRITE = 0,
    SYS_READ,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_SET_CURSOR,
    SYS_SET_COLOR,
    SYS_MMAP,
    SYS_MUNMAP,
    SYS_CREATE_PROCESS,
    SYS_EXIT,
    SYS_WAIT,
    SYS_GETPID,
    SYS_KILL,
    SYS_GETPRIORITY,
    SYS_SETPRIORITY,
    SYS_SCHED_YIELD,
    SYS_SLEEP,
    SYS_SEM_OPEN,
    SYS_SEM_CLOSE,
    SYS_SEM_WAIT,
    SYS_SEM_POST,
    SYS_SEM_GETVALUE,
    SYS_SHM_OPEN,
    SYS_SHM_UNLINK,
    SYS_SHM_MAP,
    SYS_SHM_UNMAP,
    SYS_MEM_INFO,
    SYS_BLOCK,
    SYS_UNBLOCK,
    SYS_TOGGLE_BLOCK,
    SYS_PS,
};

/**
 * @file libasm.h
 * @brief Header file for assembly functions used in the UserCodeModule.
 *
 * This header file contains the declarations of various assembly functions
 * that are used for system calls and low-level operations.
 */

/**
 * @brief Makes a system call with the specified parameters.
 *
 * @param rax The system call number.
 * @param rdi The first argument for the system call.
 * @param rsi The second argument for the system call.
 * @param rdx The third argument for the system call.
 * @param r10 The fourth argument for the system call.
 * @param r8 The fifth argument for the system call.
 * @param r9 The sixth argument for the system call.
 * @return The result of the system call.
 */
extern uint64_t sys_call(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9);

/**
 * @brief Executes an cero division exeption.
 */
extern void _div();

/**
 * @brief Executes an invalid operation exeption.
 */
extern void _ioe();

/**
 * @brief Halts the CPU.
 */
extern void _hlt();

/**
 * @brief Clears the interrupt flag, disabling interrupts.
 */
extern void _cli();

/**
 * @brief Sets the interrupt flag, enabling interrupts.
 */
extern void _sti();

#endif // LIBASM_H