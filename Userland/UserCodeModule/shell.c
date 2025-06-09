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

// Function prototypes
static int parse_command(char *input, char **argv);
static int parse_simple_pipe(char *input, char **left_cmd, char **right_cmd, int *left_argc, int *right_argc);
static command_t *find_command(const char *name);
static void read_line(char *buffer, int max_len);
static void skip_whitespace(char **str);
static int execute_pipe(char **left_cmd, char **right_cmd, int left_argc, int right_argc);

static command_t history[MAX_SAVED_COMMANDS] = {0};
static int history_count = 0;

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
    {"wc", wc, "Count lines, words, and characters in input (use 'wc --h' for options)"},
    {"filter", filter, "Filter out vowels from input"},
    {"test", test_command, "Run tests (use 'test help' for options)"},
    {NULL, NULL, NULL} // Sentinel
};

// Analizar línea de comandos en argumentos
static int parse_command(char *input, char **argv)
{
    int argc = 0;
    char *token_start;

    skip_whitespace(&input);

    while (*input != '\0' && argc < MAX_ARGS - 1)
    {
        token_start = input;

        // Encontrar el final del token actual
        while (*input != '\0' && *input != ' ' && *input != '\t')
        {
            input++;
        }

        // Terminar el token con null
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

// Buscar comando en tabla de comandos
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

// Leer una línea de entrada
static void read_line(char *buffer, int max_len)
{
    int pos = 0;
    int c;

    while (pos < max_len - 1)
    {
        c = getchar();

        // Handle Ctrl+C signal
        if (c == CHAR_INTERRUPT)
        {
            printf("^C\n");
            buffer[0] = '\0'; // Limpiar el buffer
            return;
        }

        // Handle Ctrl+D signal
        if (c == CHAR_EOF)
        {
            if (pos == 0)
            {
                printf("^D\n");
                printf("Type 'exit' to quit the shell.\n");
                buffer[0] = '\0'; // Limpiar el buffer
                return;
            }
            else
            {
                // Si hay texto en el buffer, simplemente ignorar Ctrl+D
                continue;
            }
        }

        if (c == '\n' || c == '\r')
        {
            break;
        }

        if (c == '\b' || c == 127)
        { // Backspace or DEL
            if (pos > 0)
            {
                pos--;
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
        }
        else if (c >= 32 && c <= 126)
        { // Printable characters
            buffer[pos++] = c;
            putchar(c);
        }
    }

    buffer[pos] = '\0';
    putchar('\n');
}

// Skip whitespace characters
static void skip_whitespace(char **str)
{
    while (**str == ' ' || **str == '\t')
    {
        (*str)++;
    }
}

// Parse a simple pipe between two commands (cmd1 | cmd2)
static int parse_simple_pipe(char *input, char **left_cmd, char **right_cmd, int *left_argc, int *right_argc)
{
    char *pipe_pos = strchr(input, '|');

    if (!pipe_pos)
    {
        return -1; // No pipe found
    }

    // Check for multiple pipes
    if (strchr(pipe_pos + 1, '|') != NULL)
    {
        printf("Error: Only one pipe supported at a time\n");
        return -1;
    }

    // Split the input at the pipe
    *pipe_pos = '\0';
    char *left_str = input;
    char *right_str = pipe_pos + 1;

    // Parse left command
    *left_argc = parse_command(left_str, left_cmd);
    if (*left_argc == 0)
    {
        printf("Error: Empty command before pipe\n");
        return -1;
    }

    // Parse right command
    *right_argc = parse_command(right_str, right_cmd);
    if (*right_argc == 0)
    {
        printf("Error: Empty command after pipe\n");
        return -1;
    }

    return 0;
}

// Execute a simple pipe between two commands
static int execute_pipe(char **left_cmd, char **right_cmd, int left_argc, int right_argc)
{
    // Find both commands
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

    // Create pipe
    int pipe_fd = open();
    if (pipe_fd == -1)
    {
        printf("Error: Failed to create pipe\n");
        return -1;
    }

    // Set up file descriptors for left process (writes to pipe)
    uint16_t left_fds[2];
    left_fds[0] = 0;       // STDIN
    left_fds[1] = pipe_fd; // STDOUT goes to pipe

    // Set up file descriptors for right process (reads from pipe)
    uint16_t right_fds[2];
    right_fds[0] = pipe_fd; // STDIN comes from pipe
    right_fds[1] = 1;       // STDOUT

    left_command->handler(left_argc, left_cmd, left_fds);

    right_command->handler(right_argc, right_cmd, right_fds);


    close(pipe_fd);

    return 0;
}

void shell(int argc, char **argv)
{
    char input[MAX_CMD_LENGTH];
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

        // Skip empty lines
        if (strlen(input) == 0)
        {
            continue;
        }

        // Check if the input contains a pipe
        if (strchr(input, '|') != NULL)
        {
            // Parse and execute simple pipe
            if (parse_simple_pipe(input, left_args, right_args, &left_argc, &right_argc) == 0)
            {
                execute_pipe(left_args, right_args, left_argc, right_argc);
            }
        }
        else
        {
            // Single command, parse and execute normally
            char *args[MAX_ARGS];
            int arg_count = parse_command(input, args);

            if (arg_count == 0)
            {
                continue;
            }

            // Buscar y ejecutar comando
            cmd = find_command(args[0]);
            if (cmd != NULL)
            {
                history[history_count % MAX_SAVED_COMMANDS] = *cmd; // Guardar comando en el historial
                history_count++;
                cmd->handler(arg_count, args, NULL);
            }
            else
            {
                printf("Unknown command: %s\n", args[0]);
                printf("Type 'help' for available commands.\n");
            }
        }
    }
}
