#ifndef PROGRAMS_H
#define PROGRAMS_H
void echo(int argc, char **argv);
void loop(int argc, char **argv);
void mem(int argc, char **argv);
void clear(int argc, char **argv);
void kill_shell(int argc, char **argv);
void nice_shell(int argc, char **argv);
void toggle_block_run(int argc, char **argv);
void phylo(int argc, char **argv);
void ps(int argc, char **argv);
void help(int argc, char **argv);
void shell_cmd(int argc, char **argv);
void exit_shell(int argc, char **argv);
void cat(int argc, char **argv);

typedef struct
{
    char *name;
    void (*handler)(int argc, char **argv);
    char *description;
} command_t;

#ifndef SHELL
#define SHELL
extern command_t commands[];
#endif

#endif
// PROGRAMS_H