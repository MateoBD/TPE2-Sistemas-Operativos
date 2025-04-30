/***************************************************
  libc.h
****************************************************/


#ifndef _LIBC_H_
#define _LIBC_H_
#include <stdint.h>
// #include <libasm.h>
// #include <random.h>

#define NULL (void*)0

#define DIM_X 128          // Ancho de pantalla [EN CHARS]
#define DIM_Y 48           // Alto de pantalla [EN CHARS]

#define NOT_DRAWBLE -1
#define NOT_KEY -2


/**
 * @brief Returns current saved values of CPU registers.
 * 
 * @return uint64* array with registers.
 */
uint64_t * getRegisters();

/**
 * @brief Displays the current saved values of the CPU registers.
 */
void showRegisters();

/**
 * @brief Converts the current time to a string.
 * 
 * @param buf The buffer to store the resulting string.
 */
void timeToStr(char * buf);

/**
 * @brief Halts cpu until 'q' is pressed.
 */
void hltUntilQ();

// int strStartsWith(const char * str, const char * start);

#endif
