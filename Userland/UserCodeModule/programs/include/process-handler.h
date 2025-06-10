#ifndef PROCESS_HANDLER_H
#define PROCESS_HANDLER_H
#include <stdint.h>
int8_t process_handler(char* name, void (*process)(int, char**), int argc, char **argv, uint16_t * fds);
#endif // PROCESS_HANDLER_H