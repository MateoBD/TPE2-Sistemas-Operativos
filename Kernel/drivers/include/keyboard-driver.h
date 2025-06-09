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

/**
 * @brief Retrieves the current key from the keyboard buffer.
 *
 * This function returns the current key from the keyboard buffer without
 * removing it. It can be used to peek at the current key.
 *
 * @return char The current key.
 */
// int getKey();

/**
 * @brief Checks if there is a next key available.
 *
 * @return char 1 if there is a next key, 0 otherwise.
 */
char kd_has_next_key();

/**
 * @brief Retrieves the next key from the keyboard buffer.
 *
 * @return int8_t The next key.
 */
int8_t kd_next_key();

/**
 * @brief Gets a character from the keyboard, blocking until one is available.
 *
 * @return int8_t The character from the keyboard.
 */
int8_t kd_get_char();

/**
 * @brief Clears the keyboard input buffer.
 *
 * This function clears all pending keyboard input from the STDIN pipe buffer.
 * Useful for discarding any buffered keystrokes that haven't been read yet.
 *
 * @return 0 on success, -1 on error.
 */
int kd_clear_buffer(void);

#endif // KEYBOARD_DRIVER_H