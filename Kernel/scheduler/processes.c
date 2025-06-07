#include <processes.h>
#include <memory-manager.h>
#include <pcb-queueADT.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_CHILDREN 64
#define PRIORITY_LEVELS 2
#define STACK_SIZE 0x1000 // 4KB stack size

// Estados de proceso
typedef enum ProcessState
{
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

// PCB - Process Control Block
typedef struct process_control_block
{
    pid_t pid;
    ProcessState state;
    uint8_t priority;
    void *stack_base;
    void *stack;
    struct process_control_block *father;
    struct process_control_block *children[MAX_CHILDREN];
    size_t children_count;
} PCB;

// Variables de control del scheduler
static PCB process_table[MAX_PROCESSES];
static PCBQueueADT process_queues[PRIORITY_LEVELS];
static PCBQueueADT terminated_processes_queue;
static PCB *current_process = &process_table[0]; // Proceso idle
static pid_t next_pid = 1;
static uint32_t process_count = 1;
static uint8_t initialized = 0;
uint8_t system_running = 1;

extern void *set_process_stack(int argc, char **argv, void *stack, void *entryPoint);
extern void idle_process(); // Proceso idle

int processes_initialized()
{
    return initialized;
}

int init_processes()
{
    if (initialized)
    {
        return 0;
    }

    for (uint8_t i = 0; i < PRIORITY_LEVELS; i++)
    {
        process_queues[i] = new_PCBQueueADT();
        if (process_queues[i] == NULL)
        {
            return -1;
        }
    }

    terminated_processes_queue = new_PCBQueueADT();
    if (terminated_processes_queue == NULL)
    {
        return -1;
    }

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_table[i].state = TERMINATED;
    }

    initialized = 1;

    PCB *idle_process_pcb = &process_table[0];
    idle_process_pcb->pid = 0;
    idle_process_pcb->state = READY;
    idle_process_pcb->priority = 0;
    idle_process_pcb->stack_base = (void *)memory_alloc(memory_manager, STACK_SIZE);

    process_count = 1;

    if (idle_process_pcb->stack_base == NULL)
    {
        return -1; // Error al asignar memoria para el proceso idle
    }

    idle_process_pcb->stack = set_process_stack(0, NULL, idle_process_pcb->stack_base + STACK_SIZE - 0x08, idle_process);

    return 0;
}

void set_next_process(void *current_stack)
{
    if (current_process == NULL)
    {
        return;
    }

    static uint8_t first_time = 1; // Al primer cambio de contexto, no quiero pisar el stack del idle con el del kernel
    if (first_time)
    {
        first_time = 0;
        return;
    }

    current_process->stack = current_stack;

    if (current_process->pid != 0 && current_process->state == RUNNING)
    {
        // Si el proceso actual está en ejecución, lo pasamos a READY
        current_process->state = READY;
        enqueue_process(process_queues[current_process->priority], current_process);
    }
}

void *get_idle_process_stack()
{
    return process_table[0].stack;
}

void *get_next_process()
{
    uint8_t checked_priorities[PRIORITY_LEVELS] = {0};
    uint8_t checked_count = 0;
    PCB *next_process = NULL;

    static uint64_t counter = 0; // [TO-DO] Randomizar prioridades
    counter++;
    uint8_t priority = counter % PRIORITY_LEVELS;

    while (next_process == NULL && checked_count < PRIORITY_LEVELS)
    {

        if (checked_priorities[priority])
        {
            priority = (priority + 1) % PRIORITY_LEVELS;
            continue;
        }

        checked_priorities[priority] = 1;
        checked_count++;

        int found = 0;
        while (!found)
        {
            next_process = (PCB *)dequeue_process(process_queues[priority]);
            found = (next_process == NULL) || (priority == next_process->priority && next_process->state == READY);
        }

        priority = (priority + 1) % PRIORITY_LEVELS;
    }

    // Si no se encontró ningún proceso en ninguna prioridad, usar el proceso idle
    if (next_process == NULL)
    {
        next_process = &process_table[0]; // Proceso idle
    }

    // Actualizar el proceso actual y su estado
    current_process = next_process;
    current_process->state = RUNNING;

    return current_process->stack;
}

static PCB set_new_process(uint8_t priority)
{
    PCB new_process;
    new_process.pid = next_pid++;
    new_process.state = READY;
    new_process.priority = priority;
    new_process.stack_base = (void *)memory_alloc(memory_manager, STACK_SIZE);
    new_process.stack = new_process.stack_base;
    new_process.father = current_process;
    new_process.children_count = 0;

    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        new_process.children[i] = NULL;
    }

    return new_process;
}

// Crea un nuevo proceso
pid_t create_process(void *entry_point, uint8_t priority, int argc, char **argv)
{

    if (process_count >= MAX_PROCESSES)
    {
        return -1;
    }

    int index = -1;

    for (int i = 1; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].state == TERMINATED)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        return -1; // No hay espacio para un nuevo proceso
    }

    process_table[index] = set_new_process(priority);
    PCB *new_process = &process_table[index];

    if (new_process->stack_base == NULL)
    {
        return -1; // Error al asignar memoria para el stack del proceso
    }

    if (current_process->children_count >= MAX_CHILDREN)
    {
        return -1; // El proceso padre ya tiene el maximo de hijos
    }
    current_process->children[current_process->children_count++] = new_process;

    new_process->stack = set_process_stack(argc, argv, new_process->stack_base + STACK_SIZE - 0x08, entry_point);

    enqueue_process(process_queues[priority], new_process);

    process_count++;

    return new_process->pid;
}

void free_terminated_processes(void)
{
    PCB *terminated_process = NULL;

    while ((terminated_process = (PCB *)dequeue_process(terminated_processes_queue)) != NULL)
    {
        terminated_process->state = TERMINATED;
        terminated_process->pid = 0;
        process_count--;
        // Liberar la memoria del stack del proceso terminado
        if (terminated_process->stack_base != NULL)
        {
            memory_free(memory_manager, terminated_process->stack_base);
            terminated_process->stack_base = NULL;
        }
    }
}
// Elimina un proceso
int kill_process(uint32_t pid)
{
    if (pid == 0)
    {
        return -1;
    }

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            process_table[i].state = TERMINATED;
            enqueue_process(terminated_processes_queue, &process_table[i]);
            return 0;
        }
    }

    return -1; // Proceso no encontrado
}

// Obtiene el PID del proceso actual
pid_t get_current_pid()
{
    if (current_process == NULL)
    {
        return 0;
    }
    return current_process->pid;
}

uint8_t get_current_priority(void)
{
    if (current_process == NULL)
    {
        return 0; // Proceso idle
    }
    return current_process->priority;
}

int change_priority(uint32_t pid, uint8_t newPriority)
{
    if (newPriority >= PRIORITY_LEVELS)
    {
        return -1;
    }

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (process_table[i].priority == newPriority)
            {
                return 0;
            }

            process_table[i].priority = newPriority;

            // Si el proceso está en estado READY, necesitamos encolarlo en la nueva cola
            if (process_table[i].state == READY)
            {
                enqueue_process(process_queues[newPriority], &process_table[i]);
            }

            return 0;
        }
    }

    return -1; // Proceso no encontrado
}

int block_process(pid_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (process_table[i].state == TERMINATED || process_table[i].state == BLOCKED)
            {
                return -1; // El proceso ya está bloqueado
            }

            process_table[i].state = BLOCKED;
            return 0;
        }
    }

    return -1; // Proceso no encontrado
}

int wake_up_process(pid_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (process_table[i].state == TERMINATED || process_table[i].state != BLOCKED)
            {
                return -1; // El proceso no está bloqueado
            }

            process_table[i].state = READY;
            enqueue_process(process_queues[process_table[i].priority], &process_table[i]);
            return 0;
        }
    }

    return -1; // Proceso no encontrado
}

void stop_system()
{
    system_running = 0;
}

int is_system_running()
{
    return system_running;
}

int has_running_processes()
{
    return system_running && (process_count > 1); // More than just idle process
}
