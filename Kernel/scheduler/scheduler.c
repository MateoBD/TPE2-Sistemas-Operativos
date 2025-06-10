#include <processes.h>
#include <stdint.h>
#include <stddef.h>
#include <sleep-manager.h>

void * scheduler(void * current_stack)
{
    if (!processes_initialized())
    {
        return current_stack;
    }

    if (current_stack == NULL)
    {
        return get_idle_process_stack();
    }

    free_terminated_processes();

    if (!has_running_processes()) {
        stop_system();
        return get_idle_process_stack();
    }

    set_next_process(current_stack);

    return get_next_process();
}
