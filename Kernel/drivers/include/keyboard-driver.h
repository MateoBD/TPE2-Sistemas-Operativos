#ifndef _KEYBOARD_DRIVER_H_
#define _KEYBOARD_DRIVER_H_
// Function declarations and macros for the Keyboard driver

#define NOT_DRAWBLE -1
#define NOT_KEY -2

/**
 * @brief Handles keyboard interrupts.
 * 
 * This function is called whenever a keyboard interrupt occurs.
 * It processes the key press and performs the necessary actions.
 */
void keyboard_handler();

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
char has_next_key();

/**
 * @brief Retrieves the next key from the keyboard buffer.
 * 
 * @return char The next key.
 */
int next_key();


#endif // KEYBOARD_DRIVER_H