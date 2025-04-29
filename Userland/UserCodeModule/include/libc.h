/***************************************************
  libc.h
****************************************************/


#ifndef _LIBC_H_
#define _LIBC_H_
#include <stdint.h>
// #include <libasm.h>
// #include <random.h>

#define NULL (void*)0

#define DIM_X 1024          // Ancho de pantalla [EN PIXELES]
#define DIM_Y 768           // Alto de pantalla [EN PIXELES]

#define BASE_CHAR_WIDTH 8           // Ancho de un char base [EN PIXELES]
#define BASE_CHAR_HEIGHT 16         // Ancho de un char base [EN PIXELES]

#define BASE_DIM_CHAR_Y (DIM_Y/BASE_CHAR_HEIGHT)      // Ancho de pantalla [EN CHARS BASE]
#define BASE_DIM_CHAR_X (DIM_X/BASE_CHAR_WIDTH)       // Alto de pantalla [EN CHARS BASE]

#define NOT_DRAWBLE -1
#define NOT_KEY -2

typedef struct {
	int x;
	int y;
}Point;

typedef struct {
    char day;
    char month;
    char year;
    char hour;
    char min;
    char sec;
} time;

/**
 * @brief Displays the current program time.
 */
void programTime();

/**
 * @brief Retrieves the number of ticks since the system started.
 * 
 * @return uint64_t Number of ticks.
 */
uint64_t getTicks();

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
 * @brief Retrieves the current system time.
 * 
 * @return time* Pointer to a time structure containing the current time.
 */
time * getTime();

/**
 * @brief Sets the background color of the display.
 * 
 * @param hexColor The color to set as the background, in hexadecimal format.
 */
void setBackGroundColor(uint32_t hexColor);

/**
 * @brief Draws a rectangle on the display.
 * 
 * @param topLeft The top-left corner of the rectangle.
 * @param downRigth The bottom-right corner of the rectangle.
 * @param color The color of the rectangle, in hexadecimal format.
 */
void drawRectangle(Point topLeft, Point downRigth, uint32_t color);

/**
 * @brief Draws a spray pattern on the display.
 * 
 * @param size_x The width of the spray pattern.
 * @param size_y The height of the spray pattern.
 * @param spray The spray pattern data.
 * @param mirror Whether to mirror the spray pattern.
 */
void drawSpray(uint32_t size_x, uint32_t size_y, uint32_t spray[][size_y]);

/**
 * @brief Sets the font color for text display.
 * 
 * @param hexColor The color to set for the font, in hexadecimal format.
 */
void setFontColor(uint32_t hexColor);

/**
 * @brief Prints a string to the display.
 * 
 * @param buf The string to print.
 */
void print(const char * buf);

/**
 * @brief Prints a string to the display with a specified length.
 * 
 * @param buf The string to print.
 * @param lenght The length of the string.
 */
void nprint(const char * buf, uint64_t lenght);

/**
 * @brief Reads input from the user into a buffer.
 * 
 * @param buf The buffer to store the input.
 * @param count The maximum number of characters to read.
 * @return int The number of characters read.
 */
int scan(char * buf, uint32_t count);

/**
 * @brief Converts an integer to a string.
 * 
 * @param value The integer value to convert.
 * @param buffer The buffer to store the resulting string.
 * @param base The numerical base for the conversion.
 * @param n The minimum number of digits to output.
 * @return int The length of the resulting string.
 */
int itoa(uint64_t value, char * buffer, int base, int n);

/**
 * @brief Waits for and returns a character input from the user.
 * 
 * @return char The character input by the user.
 */
char getChar();

/**
 * @brief Outputs a character to the display.
 * 
 * @param c The character to output.
 */
void putChar(char c);

/**
 * @brief Calculates the length of a string.
 * 
 * @param str The string to measure.
 * @return int The length of the string.
 */
int strlen(const char * str);

/**
 * @brief Sets the cursor position on the display.
 * 
 * @param x The x-coordinate of the cursor.
 * @param y The y-coordinate of the cursor.
 */
void setCursor(uint32_t x, uint32_t y);

/**
 * @brief Sets the character cursor position on the display.
 * 
 * @param x The x-coordinate of the character cursor.
 * @param y The y-coordinate of the character cursor.
 */
void setCharCursor(uint32_t x, uint32_t y);

/**
 * @brief Pauses execution for a specified number of seconds.
 * 
 * @param seconds The number of seconds to sleep.
 */
void sleep(uint64_t seconds);

/**
 * @brief Copies a string from source to destination.
 * 
 * @param source The source string.
 * @param dest The destination buffer.
 */
void strCpy(char * source, char * dest);

/**
 * @brief Copies up to n characters from source to destination.
 * 
 * @param source The source string.
 * @param dest The destination buffer.
 * @param n The maximum number of characters to copy.
 * @return char* The destination buffer.
 */
char* strNCpy(const char * source, char * dest, int n);

/**
 * @brief Compares two strings.
 * 
 * @param s1 The first string.
 * @param s2 The second string.
 * @return int An integer less than, equal to, or greater than zero if s1 is found, respectively, to be less than, to match, or be greater than s2.
 */
int strCmp(const char * s1,const char * s2);

/**
 * @brief Compares two strings, ignoring case.
 * 
 * @param s1 The first string.
 * @param s2 The second string.
 * @return int An integer less than, equal to, or greater than zero if s1 is found, respectively, to be less than, to match, or be greater than s2.
 */
int strCaseCmp(const char * s1, const char * s2);

/**
 * @brief Checks if a character is an alphabetic letter.
 * 
 * @param c The character to check.
 * @return int Non-zero if the character is an alphabetic letter, zero otherwise.
 */
int isalpha(int c);

/**
 * @brief Converts a character to uppercase.
 * 
 * @param c The character to convert.
 * @return int The uppercase equivalent of the character, if any; otherwise, the character itself.
 */
int toupper(int c);

/**
 * @brief Converts a character to lowercase.
 * 
 * @param c The character to convert.
 * @return int The lowercase equivalent of the character, if any; otherwise, the character itself.
 */
int tolower(int c);

/**
 * @brief Sets the zoom level for the display.
 * 
 * @param zoom The zoom level to set.
 */
void setZoom(char zoom);

/**
 * @brief Converts the current time to a string.
 * 
 * @param buf The buffer to store the resulting string.
 */
void timeToStr(char * buf);

/**
 * @brief Displays a rectangle with a specified color.
 * 
 * @param color The color of the rectangle, in hexadecimal format.
 */
void programRectangle(uint32_t color);

/**
 * @brief Displays help information for the program.
 */
void programHelp();

/**
 * @brief Displays the current values of the CPU registers.
 */
void programRegisters();

/**
 * @brief Clears the entire display.
 */
void cleanFullScreen();

/**
 * @brief Halts cpu until 'q' is pressed.
 */
void hltUntilQ();

// int strStartsWith(const char * str, const char * start);

enum colors{
    black = 0x000000,
    white = 0xFFFFFF,
    blue = 0x0000FF,
    green = 0x00FF00,
    red = 0xFF0000,
    yellow = 0xFFFF00,
    purple = 0xFF00FF,
    cyan = 0x00FFFF,
    orange = 0xFFA500,
    pink = 0xFFC0CB,
    brown = 0xA52A2A,
    lightGrey = 0xD3D3D3,
    lightBlue = 0xADD8E6,
    lightGreen = 0x90EE90,
    lightRed = 0xFFA07A,
    lightPink = 0xFFB6C1,
    lightBrown = 0xD2B48C,
    darkBlue = 0x00008B,
    darkGreen = 0x006400,
    darkRed = 0x8B0000,
    darkYellow = 0x9B870C,
    darkPurple = 0x800080,
    darkCyan = 0x008B8B,
    darkOrange = 0xFF8C00,
    darkPink = 0xFF1493,
    darkBrown = 0x654321
};

#endif
