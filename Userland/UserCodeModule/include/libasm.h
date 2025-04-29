#ifndef LIBASM_H
#define LIBASM_H

#include <stdint.h>

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
 * @param r10 The third argument for the system call.
 * @param r8 The fourth argument for the system call.
 * @return The result of the system call.
 */
extern uint64_t sys_call(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

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