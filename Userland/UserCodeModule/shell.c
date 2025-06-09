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
static command_t *find_command(const char *name);
static void read_line(char *buffer, int max_len);
static void skip_whitespace(char **str);

static command_t history[MAX_SAVED_COMMANDS] = {0};
static int history_count = 0;

command_t commands[] = {
    {"echo", echo, "Print arguments to stdout"},
    {"loop", loop, "Run a process in an infinite loop"},
    {"mem", mem, "Print memory state"},
    {"clear", clear, "Clear the screen"},
    {"kill", kill_shell, "Terminate a process by PID"},
    {"nice", nice_shell, "Change the priority of a process by PID"},
    {"block", block_shell, "Block a process by PID"},
    {"unblock", unblock_shell, "Unblock a process by PID"},
    {"phylo", phylo, "Run the philosophers problem simulation"},
    {"sh", shell, "Start the GNA shell"},
    {"help", help, "Show available commands"},
    {"exit", exit_shell, "Exit the shell"},
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

void shell(int argc, char **argv)
{
    char input[MAX_CMD_LENGTH];
    char *args[MAX_ARGS];
    int arg_count;
    command_t *cmd;

    printf("Welcome to GNA Shell!\n");
    printf("Type 'help' for available commands.\n\n");
    while (1)
    {
        printf("gna> ");
        read_line(input, MAX_CMD_LENGTH);

        // Skip empty lines
        if (strlen(input) == 0)
        {
            continue;
        }

        // Analizar comando y argumentos
        arg_count = parse_command(input, args);

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
            cmd->handler(arg_count, args);
        }
        else
        {
            printf("Unknown command: %s\n", args[0]);
            printf("Type 'help' for available commands.\n");
        }
    }
}
