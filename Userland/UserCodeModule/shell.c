#define SHELL
#include <shell.h>
#include <gnaio.h>
#include <gnalib.h>
#include <gnastring.h>
#include <stdint.h>
#include <programs.h>
#include <gnauni.h>
#include <test-programs.h>

#define MAX_CMD_LENGTH 256
#define MAX_ARGS 16
#define MAX_SAVED_COMMANDS 16

#define IS_PRINTABLE(c) ((c) >= ' ' && (c) <= '~')

static int parse_command(char *input, char **argv);
static int parse_simple_pipe(char *input, char **left_cmd, char **right_cmd, int *left_argc, int *right_argc);
static command_t *find_command(const char *name);
static void read_line(char *buffer, int max_len);
static void skip_whitespace(char **str);
static int execute_pipe(char **left_cmd, char **right_cmd, int left_argc, int right_argc);

command_t commands[] = {
    {"echo", echo, "Print arguments to stdout"},
    {"loop", loop, "Run a process in an infinite loop"},
    {"mem", mem, "Print memory state"},
    {"clear", clear, "Clear the screen"},
    {"kill", kill_shell, "Terminate a process by PID"},
    {"nice", nice_shell, "Change the priority of a process by PID"},
    {"block", toggle_block_run, "Toggle block/unblock a process by PID"},
    {"phylo", phylo, "Run the philosophers problem simulation"},
    {"ps", ps, "Show current processes"},
    {"sh", shell_cmd, "Start the GNA shell"},
    {"help", help, "Show available commands"},
    {"exit", exit_shell, "Exit the shell"},
    {"cat", cat, "Concatenate and print files to stdout"},
    {"wc", wc, "Count lines, words, and characters in input (use 'wc -h' for options)"},
    {"filter", filter, "Filter out vowels from input"},
    {"test", test_command, "Run tests (use 'test -h' or 'test --help' for options)"},
    {NULL, NULL, NULL} 
};

static int parse_command(char *input, char **argv)
{
    int argc = 0;
    char *token_start;

    skip_whitespace(&input);

    while (*input != '\0' && argc < MAX_ARGS - 1)
    {
        token_start = input;

        while ((*input != '\0') && (*input != ' '))
        {
            input++;
        }

        if (*input != '\0')
        {
            *input = '\0';
            input++;
        }

        argv[argc++] = token_start;
        skip_whitespace(&input);
    }

    argv[argc] = NULL;
    return argc;
}

static command_t *find_command(const char *name)
{
    for (int i = 0; commands[i].name != NULL; i++)
    {
        if (strcmp(commands[i].name, name) == 0)
        {
            return &commands[i];
        }
    }
    return NULL;
}

static void read_line(char *buffer, int max_len)
{
    int pos = 0;
    int c;

    while ((pos < max_len - 1) && (c = getchar()) != '\n')
    {
        if (c == CHAR_INTERRUPT)
        {
            printf("^C\n");
            buffer[0] = '\0'; 
            return;
        }

        if ((c == CHAR_EOF) && (pos == 0))
        {
                printf("^D\n");
                printf("Type 'exit' to quit the shell.\n");
                buffer[0] = '\0'; 
                return;
        }

        if (c == '\b')
        {
            if (pos > 0)
            {
                pos--;
                putchar('\b');
            }
        }

        if (IS_PRINTABLE(c))
        {
            buffer[pos++] = c;
            putchar(c);
        }
    }
    buffer[pos] = '\0';
    putchar('\n');
}

static void skip_whitespace(char **str)
{
    while (**str == ' ')
    {
        (*str)++;
    }
}

static int parse_simple_pipe(char *input, char **left_cmd, char **right_cmd, int *left_argc, int *right_argc)
{
    char *pipe_pos = strchr(input, '|');

    if (!pipe_pos)
    {
        return -1; 
    }

    if (strchr(pipe_pos + 1, '|') != NULL)
    {
        printf("Error: Only one pipe supported at a time\n");
        return -1;
    }

    *pipe_pos = '\0';
    char *left_str = input;
    char *right_str = pipe_pos + 1;
    uint32_t right_len = strlen(right_str);
    right_str[right_len] = ' ';
    right_str[right_len + 1] = '&';
    right_str[right_len + 2] = '\0';

    *left_argc = parse_command(left_str, left_cmd);
    if (*left_argc == 0)
    {
        printf("Error: Empty command before pipe\n");
        return -1;
    }

    *right_argc = parse_command(right_str, right_cmd);
    if (*right_argc == 0)
    {
        printf("Error: Empty command after pipe\n");
        return -1;
    }

    return 0;
}

static int execute_pipe(char **left_cmd, char **right_cmd, int left_argc, int right_argc)
{
    command_t *left_command = find_command(left_cmd[0]);
    command_t *right_command = find_command(right_cmd[0]);

    if (left_command == NULL)
    {
        printf("Unknown command: %s\n", left_cmd[0]);
        return -1;
    }

    if (right_command == NULL)
    {
        printf("Unknown command: %s\n", right_cmd[0]);
        return -1;
    }

    int pipe_fd = open();
    if (pipe_fd == -1)
    {
        printf("Error: Failed to create pipe\n");
        return -1;
    }

    uint16_t left_fds[2];
    left_fds[0] = 0;
    left_fds[1] = pipe_fd; 

    uint16_t right_fds[2];
    right_fds[0] = pipe_fd;
    right_fds[1] = 1;

    right_command->handler(right_argc, right_cmd, right_fds);

    left_command->handler(left_argc, left_cmd, left_fds);

    close(pipe_fd);

    return 0;
}

void shell(int argc, char **argv)
{
    char input[MAX_CMD_LENGTH + 2];
    char *left_args[MAX_ARGS];
    char *right_args[MAX_ARGS];
    int left_argc, right_argc;
    command_t *cmd;

    printf("Welcome to GNA Shell!\n");
    printf("Type 'help' for available commands.\n");
    printf("Use '|' to pipe one command to another (cmd1 | cmd2).\n\n");

    while (1)
    {
        printf("gna> ");
        read_line(input, MAX_CMD_LENGTH);

        if (strlen(input) == 0)
        {
            continue;   
        }     
        if ((strchr(input, '|') != NULL) && (parse_simple_pipe(input, left_args, right_args, &left_argc, &right_argc) == 0))
        {
            execute_pipe(left_args, right_args, left_argc, right_argc);
            continue;
        }
        char *args[MAX_ARGS];
        int arg_count = parse_command(input, args);

        if ((arg_count != 0) && (cmd = find_command(args[0])) != NULL)
        {
            cmd->handler(arg_count, args, NULL);
            continue;
        }
        printf("Unknown command: %s\n", args[0]);
        printf("Type 'help' for available commands.\n");
    }
}
