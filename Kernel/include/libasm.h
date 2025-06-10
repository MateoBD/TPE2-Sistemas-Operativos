#ifndef _LIBASM_H_
#define _LIBASM_H_

/**
 * @file libasm.h
 * @brief Header file for assembly-related functions and system calls.
 */


void _hlt(void);
void _sti(void);
void _cli(void);

/**
 * @brief Outputs a byte to the specified port.
 */
void outb(unsigned short port, unsigned char value);

/**
 * @brief Inputs a byte from the specified port.
 */
unsigned char inb(unsigned short port);

/**
 * @brief Retrieves the CPU vendor string.
 */
void cpu_vendor(char *vendor);

/**
 * @brief Tests a system call.
 */
void test_sys_call(void);

/**
 * @brief Retrieves the current CPU register values.
 */
void get_registers(unsigned long *registers);

void haltcpu(void);

#endif // _LIBASM_H_
