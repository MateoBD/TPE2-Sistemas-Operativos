#include <processes.h>
#include <stdint.h>
#include <stddef.h>

void * scheduler(void * current_stack)
{
    if (!processes_initialized())
    {
        return current_stack;
    }

    if (current_stack == NULL)
    {
        return get_idle_process_stack(); // Retornar el idle process si no se estan ejecutando procesos
    }

    free_terminated_processes();

    set_next_process(current_stack);

    return get_next_process();
}
