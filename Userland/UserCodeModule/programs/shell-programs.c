#include <gnaio.h>
#include <gnalib.h>
#include <programs.h>
#include <gnauni.h>
#include <process-handler.h>
#include <shell.h>

#define BUFFER_SIZE 1024

void cmd_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        printf("%s", argv[i]);
        if (i < argc - 1)
        {
            putchar(' ');
        }
    }
    putchar('\n');
    exit(0);
}

void cmd_loop(int argc, char **argv)
{
    uint32_t pid = get_pid();
    while (1)
    {
        printf("Process %d started in loop mode.\n", pid);
        sleep(1);
    }
    exit(0);
}

void cmd_help(int argc, char **argv)
{
    printf("Available commands:\n");
    for (int i = 0; commands[i].name != NULL; i++)
    {
        printf("  %s - %s\n", commands[i].name, commands[i].description);
    }
    exit(0);
}

void nice_shell(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: nice <pid> <new_priority>\n");
        return;
    }

    uint16_t pid = atoi(argv[1]);
    if (pid == 0)
    {
        printf("Invalid PID: %s\n", argv[1]);
        return;
    }

    uint8_t new_priority = atoi(argv[2]);
    if (new_priority > 255)
    {
        printf("Invalid priority: %s\n", argv[2]);
        return;
    }

    if (nice(pid, new_priority) == -1)
    {
        printf("Failed to change priority of process %d\n", pid);
    }
    else
    {
        printf("Process %d priority changed to %d successfully.\n", pid, new_priority);
    }
}

void exit_shell(int argc, char **argv)
{
    printf("Goodbye!\n");
    exit(0);
}

void echo(int argc, char **argv)
{
    process_handler("echo", cmd_echo, argc, argv);
}

void loop(int argc, char **argv)
{
    process_handler("loop", cmd_loop, argc, argv);
}

void kill_shell(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: kill <pid>\n");
        return;
    }

    uint16_t pid = atoi(argv[1]);
    if (pid == 0)
    {
        printf("Invalid PID: %s\n", argv[1]);
        return;
    }

    if (kill(pid) == -1)
    {
        printf("Failed to kill process %d\n", pid);
    }
    else
    {
        printf("Process %d killed successfully.\n", pid);
    }
}

void clear(int argc, char **argv)
{
    clean_screen();
}

void help(int argc, char **argv)
{
    process_handler("help", cmd_help, argc, argv);
}

static const char *process_state_to_string(int state)
{
    switch (state)
    {
    case PS_READY: return "READY";
    case PS_RUNNING: return "RUNNING";
    case PS_BLOCKED: return "BLOCKED";
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

    // Header
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

    exit(0);
}

void ps(int argc, char **argv)
{
    process_handler("ps", cmd_ps, argc, argv);
}

void shell_cmd(int argc, char **argv)
{
    // Start the shell program
    process_handler("shell", shell, argc, argv);
}


void cat_cmd(int argc, char **argv) {
  int c;
  char buff[BUFFER_SIZE];
  uint16_t count = 0;
  while ((c = getchar()) != EOF) {
    buff[count++] = c;
    if (c == '\n' || count - 1 >= BUFFER_SIZE) {
      buff[count] = 0;
      printf("%s", buff);
      count = 0;
    }
  }
  buff[count] = 0;
  printf("%s", buff);
  count = 0;
}

void cat(int argc, char **argv)
{
    process_handler("cat", cat_cmd, argc, argv);
}

void wc_cmd(int argc, char **argv)
{
    printf("wc command is not implemented yet.\n");
    exit(0); // Placeholder for wc command implementation
}

void wc(int argc, char **argv)
{
    process_handler("wc", wc_cmd, argc, argv);
}

void filter_cmd(int argc, char **argv)
{
    printf("filter command is not implemented yet.\n");
    exit(0); // Placeholder for filter command implementation
}

void filter(int argc, char **argv)
{
    process_handler("filter", filter_cmd, argc, argv);
}

void toggle_block_run(int argc, char **argv)
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
    exit(0);
}