#ifndef SNAKE_H
#define SNAKE_H
#include <libc.h>

// Direction enumeration for snake movement
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

// Structure to represent the snake
typedef struct {
    int body[400];      // Array of blocks representing the snake's body
    int head;           //index of the head
    int tail;           //index of the tail
    int length;         // Current length of the snake
    int player;         // Player number
    Direction dir;      // Current direction of the snake
    uint32_t color;     // Color of the snake
    int toGrow;         // Number of blocks to grow
} Snake;

/**
 * @brief Starts and runs the Snake game.
 *
 * This function initializes and manages the game loop for the Snake game.
 * It handles the game state, user input, and rendering of the game.
 */
void snake();

#define CHAR_WIDTH_ 8
#define CHAR_HEIGHT_ 16

#endif // SNAKE_H