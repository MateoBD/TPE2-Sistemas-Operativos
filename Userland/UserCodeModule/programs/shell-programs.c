#include <gnaio.h>
#include <gnalib.h>
#include <programs.h>
#include <gnauni.h>

void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) {
            putchar(' ');
        }
    }
    putchar('\n');
    exit(0);
}

void cmd_clear(int argc, char **argv) {
    clean_screen();
    exit(0);
}

// Help command implementation
void cmd_help(int argc, char **argv) {
    printf("Available commands:\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        printf("  %s - %s\n", commands[i].name, commands[i].description);
    }
    exit(0);
}

// Exit command implementation
void exit_shell(int argc, char **argv) {
    printf("Goodbye!\n");
    exit(0);
}

void echo(int argc, char **argv){
    uint16_t pid = create_process("echo", cmd_echo, argc, argv, NULL);
    
    int8_t exit_status = -1;
    wait(pid, &exit_status);
    printf("Exit status: %d\n", exit_status);
}

void clear(int argc, char **argv){
    create_process("clear", cmd_clear, argc, argv, NULL);
}

void help(int argc, char **argv){
    create_process("help", cmd_help, argc, argv, NULL);
}