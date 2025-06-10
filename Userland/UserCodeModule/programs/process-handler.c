#include <gnauni.h>
#include <stddef.h>

int8_t process_handler(char* name, void (*process)(int, char**), int argc, char **argv, uint16_t * fds)
{
    uint8_t foreground = 1;
    if (argc > 0 && argv[argc - 1] != NULL && argv[argc - 1][0] == '&')
    {
        foreground = 0;
        argc--;
    }
    return create_process(name, process, argc, argv, fds, foreground);
}
