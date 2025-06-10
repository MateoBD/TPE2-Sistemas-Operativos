#ifndef _LIB_H_
#define _LIB_H_

#include <stdint.h>

/**
 * @file lib.h
 * @brief Header file for common library functions.
 */

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

char *cpu_vendor(char *result);

uint32_t strlen(const char *str);

uint32_t strncpy(char *destination, const char *source, uint32_t length);

#endif // _LIB_H_