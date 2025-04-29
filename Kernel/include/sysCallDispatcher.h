#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h> 
#include <stdlib.h>

typedef enum{
    STDIN = 0,
    STDOUT,
    STDERR,
} FDS;

uint64_t sysCallDispatcher(uint64_t rax, ...);

size_t sys_setCursor(int x, int y);
void setFontColor(uint32_t hexColor);
void sys_write(FDS fd, const char *buf, size_t count);
uint32_t readChars(char *buf, size_t count);
size_t sys_read(FDS fd, char *buf, size_t count);
void sys_sleep(int seconds);
void sys_setZoom(int new_zoom);
void showRegisters();
int strLen(char * str);

#endif