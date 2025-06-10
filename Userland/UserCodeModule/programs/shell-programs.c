#include <gnaio.h>
#include <gnalib.h>
#include <programs.h>
#include <gnauni.h>
#include <process-handler.h>
#include <shell.h>
#include <gnastring.h>

#define BUFFER_SIZE 1024

void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        printf("%s", argv[i]);
        if (i < argc - 1) {
            putchar(' ');
        }
    }
    putchar('\n');
    putchar(CHAR_EOF);
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
    putchar(CHAR_EOF);
    exit(0);
}

void exit_shell(int argc, char **argv, uint16_t * fds) {
    printf("Goodbye!\n");
    exit(0);
}

void echo(int argc, char **argv, uint16_t * fds){
    process_handler("echo", cmd_echo, argc, argv, fds);
}

void loop(int argc, char **argv, uint16_t * fds){
    process_handler("loop", cmd_loop, argc, argv, fds);
}

void kill_shell(int argc, char **argv, uint16_t * fds){
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

void clear(int argc, char **argv, uint16_t * fds){
    clean_screen();
}

void help(int argc, char **argv, uint16_t * fds){
    process_handler("help", cmd_help, argc, argv, fds);
}

static const char *process_state_to_string(int state)
{
    switch (state)
    {
    case PS_READY: return "READY";
    case PS_RUNNING: return "RUNNING";
    case PS_BLOCKED: return "BLOCKED";
    case PS_SLEEPING: return "SLEEPING";
    case PS_TERMINATED: return "TERMINATED";
    default: return "UNKNOWN";
    }
}



void cmd_ps(int argc, char **argv)
{
    printf("Current processes:\n");
    ProcessInfo processes[MAX_PROCESSES];
    int count = get_ps(processes, MAX_PROCESSES);
    if (count == -1)
    {
        printf("Failed to get process list.\n");
        return;
    }

    printf("PID   Name       State     Priority  Parent  FG\n");

    for (int i = 0; i < count; i++)
    {
        set_color(lightMagenta,black);
        print_int_padded(processes[i].pid, 6);
        set_color(lightCyan,black);
        print_padded(processes[i].name, 11);
        set_color(lightGreen,black);
        print_padded(process_state_to_string(processes[i].state), 10);
        set_color(lightBlue,black);
        print_int_padded(processes[i].priority, 10);
        set_color(lightRed,black);
        print_int_padded(processes[i].parent_pid, 8);
        set_color(yellow,black);
        print_int_padded(processes[i].is_foreground, 3);
        set_color(lightGrey,black);
        putchar('\n');
    }
    putchar(CHAR_EOF);
    exit(0);
}

void ps(int argc, char **argv, uint16_t * fds)
{
    process_handler("ps", cmd_ps, argc, argv, fds);
}

void shell_cmd(int argc, char **argv, uint16_t * fds)
{
    process_handler("shell", shell, argc, argv, fds);
}


void cat_cmd(int argc, char **argv) 
{
    int c;
    char buff[BUFFER_SIZE];
    uint16_t count = 0;
    while ((c = getchar()) != CHAR_EOF) 
    {
        printf("%c",c);
        buff[count++] = c;
        if (c == '\n' || count - 1 >= BUFFER_SIZE) 
        {
            buff[count] = 0;
            printf("%s", buff);
            count = 0;
        }
    }
    buff[count] = 0;
    printf("%s\n", buff);
    count = 0;
    putchar(CHAR_EOF);
    exit(0); 
}

void cat(int argc, char **argv, uint16_t * fds)
{
    process_handler("cat", cat_cmd, argc, argv, fds);
}

void wc_cmd(int argc, char **argv)
{
    int show_lines = 0, show_words = 0, show_chars = 0;

    if (argc == 1)
    {
        show_lines = show_words = show_chars = 1;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-l") == 0) show_lines = 1;
            else if (strcmp(argv[i], "-w") == 0) show_words = 1;
            else if (strcmp(argv[i], "-c") == 0) show_chars = 1;
            else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
            {
                printf("Usage: wc [options]\n");
                printf("Options:\n");
                printf("  -l    Count lines\n");
                printf("  -w    Count words\n");
                printf("  -c    Count characters\n");
                printf("  --help, -h    Show this help message\n");
                putchar(CHAR_EOF);
                exit(0);
            }
            else
            {
                printf("Unknown option: %s\n", argv[i]);
                putchar(CHAR_EOF);
                exit(1);
            }
        }
    }

    int c;
    uint16_t count_lines = 0;
    uint16_t count_words = 0;
    uint16_t count_chars = 0;
    int in_word = 0;

    while ((c = getchar()) != CHAR_EOF)
    {
        printf("%c", c);
        count_chars++;

        if (c == '\n') count_lines++;

        if (c == ' ' || c == '\n' || c == '\t')
        {
            in_word = 0;
        }
        else if (!in_word)
        {
            count_words++;
            in_word = 1;
        }
    }

    printf("\n");
    if (show_lines)  printf("Lines: %d ", count_lines);
    if (show_words)  printf("Words: %d ", count_words);
    if (show_chars)  printf("Chars: %d ", count_chars);
    printf("\n");
    putchar(CHAR_EOF);
    exit(0);
}



void wc(int argc, char **argv, uint16_t * fds)
{
    process_handler("wc", wc_cmd, argc, argv, fds);
}

void filter_cmd(int argc, char **argv)
{
    int c;
    char buff[BUFFER_SIZE];
    uint16_t count = 0;
    while ((c = getchar()) != CHAR_EOF) 
    {
        printf("%c",c);
        if (!is_vowel(c))
        {
            buff[count++] = c;
        }
    }
    buff[count] = 0;
    printf("%s\n", buff);
    putchar(CHAR_EOF);
    exit(0); 
}

void filter(int argc, char **argv, uint16_t * fds)
{
    process_handler("filter", filter_cmd, argc, argv, fds);
}

void toggle_block_run(int argc, char **argv, uint16_t * fds)
{
    if (argc < 2)
    {
        printf("Usage: toggle_block <pid>\n");
        return;
    }

    uint16_t pid = atoi(argv[1]);
    if (pid == 0)
    {
        printf("Invalid PID: %s\n", argv[1]);
        return;
    }

    if (toggle_block(pid) == -1)
    {
        printf("Failed to toggle block state of process %d\n", pid);
    }
    else
    {
        printf("Process %d block state toggled successfully.\n", pid);
    }
    putchar(CHAR_EOF);
}

void nice_cmd(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: nice <pid> <priority>\n");
        return;
    }

    uint16_t pid = atoi(argv[1]);
    int priority = atoi(argv[2]);

    if (pid == 0 || priority < 0 || priority > 3)
    {
        printf("Invalid arguments: %s %s\n", argv[1], argv[2]);
        return;
    }

    if (nice(pid, priority) == -1)
    {
        printf("Failed to change priority of process %d\n", pid);
    }
    else
    {
        printf("Process %d priority changed to %d successfully.\n", pid, priority);
    }
    putchar(CHAR_EOF);
    exit(0);
}
void nice_shell(int argc, char **argv, uint16_t * fds)
{
    process_handler("nice", nice_cmd, argc, argv, fds);
}
