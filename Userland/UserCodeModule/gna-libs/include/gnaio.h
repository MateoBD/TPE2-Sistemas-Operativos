// gnaio.h
//
// Libreria para la gestion de IO de GNA
#ifndef _GNAIO_H_
#define _GNAIO_H_

#include <stdint.h>

#define EOF -1
#define CHAR_INTERRUPT -3
#define CHAR_EOF -4
#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2
#ifndef NULL
#define NULL ((void *)0)
#endif

enum colors
{
    black = 0x00,
    blue = 0x01,
    green = 0x02,
    cyan = 0x03,
    red = 0x04,
    magenta = 0x05,
    brown = 0x06,
    lightGrey = 0x07,
    darkGrey = 0x08,
    lightBlue = 0x09,
    lightGreen = 0x0A,
    lightCyan = 0x0B,
    lightRed = 0x0C,
    lightMagenta = 0x0D,
    yellow = 0x0E,
    white = 0x0F,
};

int printf(const char * format, ...);

int nprintf(uint32_t size, const char * format, ...);

int fprintf(int fd, const char * format, ...);

int sprintf(char * buf, const char * format, ...);

int snprintf(char * buf, uint32_t size, const char * format, ...);

// int scanf(const char * format, ...);

// int fscanf(int fd, const char * format, ...);

// int sscanf(const char * buf, const char * format, ...);

// int sncanf(const char * buf, int size, const char * format, ...);

void puts(const char * str);

void putchar(char c);

int putc(int fd, char c);

int getchar(void);

int getc(int fd);

int set_cursor(uint8_t x, uint8_t y);

void set_color(uint8_t fg_color, uint8_t bg_color);

void clean_screen(void);

void print_spaces(int n);

void print_padded(const char *str, int width);

void print_int_padded(int num, int width);

#endif