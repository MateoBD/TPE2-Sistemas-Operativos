#include <gnaio.h>
#include <gnalib.h>
#include <programs.h>
#include <gnauni.h>
#include <process-handler.h>

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

void cmd_loop(int argc, char **argv) {
    uint32_t pid = get_pid();
    while (1)
    {
        printf("Process %d started in loop mode.\n", pid);
        sleep(1);
    }
    exit(0);
}

void cmd_help(int argc, char **argv) {
    printf("Available commands:\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        printf("  %s - %s\n", commands[i].name, commands[i].description);
    }
    exit(0);
}

void exit_shell(int argc, char **argv) {
    printf("Goodbye!\n");
    exit(0);
}

void echo(int argc, char **argv){
    process_handler("echo", cmd_echo, argc, argv);
}

void loop(int argc, char **argv){
    process_handler("loop", cmd_loop, argc, argv);
}

void kill_shell(int argc, char **argv){
    if (argc < 2) {
        printf("Usage: kill <pid>\n");
        return;
    }
    
    uint16_t pid = atoi(argv[1]);
    if (pid == 0) {
        printf("Invalid PID: %s\n", argv[1]);
        return;
    }
    
    if (kill(pid) == -1) {
        printf("Failed to kill process %d\n", pid);
    } else {
        printf("Process %d killed successfully.\n", pid);
    }
}

void clear(int argc, char **argv){
    clean_screen();
}

void help(int argc, char **argv){
    process_handler("help", cmd_help, argc, argv);
}
