#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

void vd_print(const char *string);

void vd_nprint(const char *string, uint32_t n);

void vd_draw_char(char character);

void vd_clear_screen();

void vd_set_cursor(uint32_t x, uint32_t y);

void vd_set_color(uint8_t new_color);

uint8_t vd_get_color(void);

void vd_print_dec(uint64_t value);

void vd_print_hex(uint64_t value);

void vd_print_bin(uint64_t value);

void vd_print_base(uint64_t value, uint32_t base);

#endif