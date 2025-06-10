#ifndef _KEYBOARD_DRIVER_H_
#define _KEYBOARD_DRIVER_H_
#include <stdint.h>
// Function declarations and macros for the Keyboard driver

#define NOT_DRAWBLE -1
#define NOT_KEY -2
#define CHAR_INTERRUPT -3
#define CHAR_EOF -4

/**
 * @brief Handles keyboard interrupts.
 *
 * This function is called whenever a keyboard interrupt occurs.
 * It processes the key press and performs the necessary actions.
 */
void kd_handler();

#endif // KEYBOARD_DRIVER_H