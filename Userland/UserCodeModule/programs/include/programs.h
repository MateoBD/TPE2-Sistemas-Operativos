#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <stdint.h>



void nice_shell(int argc, char **argv, uint16_t * fds);
void toggle_block_run(int argc, char **argv, uint16_t * fds);
void phylo(int argc, char **argv, uint16_t * fds);
void ps(int argc, char **argv, uint16_t * fds);

void shell_cmd(int argc, char **argv, uint16_t * fds);

void cat(int argc, char **argv, uint16_t * fds);
void wc(int argc, char **argv, uint16_t * fds);
void filter(int argc, char **argv, uint16_t * fds);

void echo(int argc, char **argv, uint16_t *fds);
void loop(int argc, char **argv, uint16_t *fds);
void mem(int argc, char **argv, uint16_t *fds);
void clear(int argc, char **argv, uint16_t *fds);
void kill_shell(int argc, char **argv, uint16_t *fds);
void help(int argc, char **argv, uint16_t *fds);
void exit_shell(int argc, char **argv, uint16_t *fds);

typedef struct {
    char *name;
    void (*handler)(int argc, char **argv, uint16_t *fds);
    char *description;
} command_t;

#ifndef SHELL
#define SHELL
extern command_t commands[];
#endif

#endif 
// PROGRAMS_H