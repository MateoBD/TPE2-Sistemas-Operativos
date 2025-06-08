#include <gnauni.h>
#include <stddef.h>

int8_t process_handler(char* name, void (*process)(int, char**), int argc, char **argv)
{
    uint8_t foreground = 1;
    if (argc > 0 && argv[argc - 1] != NULL && argv[argc - 1][0] == '&')
    {
        foreground = 0; // Run in background
        argc--; // Remove the '&' argument
    }
    uint16_t pid = create_process(name, process, argc, argv, NULL);
    if (foreground)
    {
        int8_t exit_status = -1;
        wait(pid, &exit_status);
        return exit_status;
    }
    return (int8_t)pid;
}
