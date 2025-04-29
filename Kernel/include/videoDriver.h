#ifndef VIDEODRIVER_H
#define VIDEODRIVER_H
#include <stdint.h>
// Function declarations and macros for the video driver

#define DIM_X 1024
#define DIM_Y 768

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

typedef struct {
	int x;
	int y;
}Point;
/**
 * @brief Draws a pixel on the screen at the specified coordinates with the given color.
 *
 * @param hexColor The color of the pixel in hexadecimal format.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 */
/**
 * @brief Puts a pixel on the screen at the specified coordinates with the given color.
 * 
 * @param hexColor The color of the pixel in hexadecimal format.
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 */
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

/**
 * @brief Draws a filled rectangle on the screen.
 * 
 * @param topLeft Pointer to the top-left corner point of the rectangle.
 * @param downRigth Pointer to the bottom-right corner point of the rectangle.
 * @param c The color of the rectangle in hexadecimal format.
 */
void drawRectangle(Point* topLeft, Point* downRigth, uint32_t c);

/**
 * @brief Draws a character on the screen with specified foreground and background colors.
 * 
 * @param c The character to be drawn.
 * @param x The x-coordinate of the character's position.
 * @param y The y-coordinate of the character's position.
 * @param fgcolor The foreground color of the character in hexadecimal format.
 * @param bgcolor The background color of the character in hexadecimal format.
 * @param mult The size multiplier for the character.
 */
void drawchar(unsigned char c, int x, int y, int fgcolor, int bgcolor, int mult);

/**
 * @brief Puts a character on the screen at the specified coordinates.
 * 
 * @param c The character to be put on the screen.
 * @param x The x-coordinate of the character's position.
 * @param y The y-coordinate of the character's position.
 */
void putchar(char c, int x, int y);

/**
 * @brief Draws a spray pattern on the screen.
 * 
 * @param size_x The width of the spray pattern.
 * @param size_y The height of the spray pattern.
 * @param spray 2D array representing the spray pattern.
 * @param cursorX The x-coordinate of the spray's position.
 * @param cursorY The y-coordinate of the spray's position.
 */
void drawSpray(int size_x, int size_y, uint32_t spray[][size_x], int cursorX, int cursorY);

#endif // VIDEODRIVER_H