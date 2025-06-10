#include <processes.h>
#include <memory-manager.h>
#include <pcb-queueADT.h>
#include <stddef.h>
#include <stdint.h>
#include <lib.h>
#include <syscalls.h>
#include <semaphores.h>
#include <random.h>
#include <sleep-manager.h>

#define MAX_CHILDREN 64
#define PRIORITY_LEVELS 3
#define STACK_SIZE 0x1000 // Tamaño del stack 4KB
#define MAX_PROCESS_NAME_LENGTH 64

typedef enum ProcessState
{
    READY,
    RUNNING,
    BLOCKED,
    SLEEPING,
    TERMINATED
} ProcessState;

typedef struct process_control_block
{
    pid_t pid;
    char name[MAX_PROCESS_NAME_LENGTH];
    ProcessState state;
    ProcessState previous;
    uint8_t priority;
    void *stack_base;
    void *stack;
    uint16_t fds[2]; // File descriptors: [0] stdin, [1] stdout
    struct process_control_block *father;
    struct process_control_block *children[MAX_CHILDREN];
    size_t children_count;
    int8_t exit_status;
    int32_t sem_wait_id;
    uint8_t is_foreground;
} PCB;


static PCB process_table[MAX_PROCESSES];
static PCBQueueADT process_queues[PRIORITY_LEVELS];
static PCBQueueADT terminated_processes_queue;
static PCB *current_process = &process_table[0]; // Proceso idle
static pid_t next_pid = 0;
static uint32_t process_count = 1;
static uint8_t initialized = 0;
uint8_t system_running = 1;
static pid_t foreground_process = 0;

extern void *set_process_stack(int argc, char **argv, void *stack, void *entryPoint);
extern void idle_process(); 

static PCB set_new_process(const char *name, uint16_t *fds, uint8_t is_foreground)
{
    PCB new_process;
    new_process.pid = next_pid++;

    if (name != NULL)
    {
        strncpy(new_process.name, name, MAX_PROCESS_NAME_LENGTH - 1);
        new_process.name[MAX_PROCESS_NAME_LENGTH - 1] = '\0'; // Asegurar terminación null
    }
    else
    {
        strncpy(new_process.name, "unnamed", MAX_PROCESS_NAME_LENGTH - 1);
        new_process.name[MAX_PROCESS_NAME_LENGTH - 1] = '\0';
    }

    new_process.state = READY;
    new_process.previous = TERMINATED;
    new_process.priority = 0;
    new_process.stack_base = (void *)memory_alloc(memory_manager, STACK_SIZE);
    new_process.stack = new_process.stack_base;
    new_process.father = current_process;
    new_process.fds[STDIN] = (fds != NULL && fds[STDIN] != 0) ? fds[STDIN] : STDIN;
    new_process.fds[STDOUT] = (fds != NULL && fds[STDOUT] != 0) ? fds[STDOUT] : STDOUT;
    new_process.children_count = 0;

    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        new_process.children[i] = NULL;
    }

    new_process.exit_status = -1;
    new_process.sem_wait_id = -1;
    new_process.is_foreground = is_foreground;

    return new_process;
}

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

    process_table[0] = set_new_process("init", NULL, 1);
    PCB *idle_process_pcb = &process_table[0];

    process_count = 1;

    if (idle_process_pcb->stack_base == NULL)
    {
        return -1;
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

    if (current_process->state == RUNNING)
    {
        current_process->state = READY;
        if (current_process->pid != 0)
        {
            enqueue_process(process_queues[current_process->priority], current_process);
        }
    }
}

void *get_idle_process_stack()
{
    return process_table[0].stack;
}

static uint8_t get_random_priority()
{

    uint32_t total_weight = 0;
    for (uint8_t i = 0; i < PRIORITY_LEVELS; i++)
    {
        total_weight += (PRIORITY_LEVELS - i) * (PRIORITY_LEVELS - i);
    }

    uint32_t random_value = randInt(0, total_weight - 1);

    uint32_t accumulated_weight = 0;
    for (uint8_t priority = 0; priority < PRIORITY_LEVELS; priority++)
    {
        accumulated_weight += (PRIORITY_LEVELS - priority) * (PRIORITY_LEVELS - priority);
        if (random_value < accumulated_weight)
        {
            return priority;
        }
    }

    return PRIORITY_LEVELS - 1;
}

void *get_next_process()
{
    uint8_t checked_priorities[PRIORITY_LEVELS] = {0};
    uint8_t checked_count = 0;
    PCB *next_process = NULL;

    uint8_t priority = get_random_priority();

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

    if (next_process == NULL)
    {
        next_process = &process_table[0];
    }

    current_process = next_process;
    current_process->state = RUNNING;

    return current_process->stack;
}

pid_t create_process(const char *name, void *entry_point, int argc, char **argv, uint16_t *fds, uint8_t is_foreground)
{

    if (!initialized)
    {
        if (init_processes() == -1)
        {
            return -1;
        }
    }

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
        return -1; 
    }

    if (is_foreground)
    {
        if (!current_process->is_foreground)
        {
            is_foreground = 0; 
        }
        else
        {
            current_process->is_foreground = 0;
            foreground_process = next_pid;
        }
    }

    process_table[index] = set_new_process(name, fds, is_foreground);
    PCB *new_process = &process_table[index];

    if (new_process->stack_base == NULL)
    {
        return -1; 
    }

    if (current_process->children_count >= MAX_CHILDREN)
    {
        return -1; 
    }

    current_process->children[current_process->children_count++] = new_process;

    new_process->stack = set_process_stack(argc, argv, new_process->stack_base + STACK_SIZE - 0x08, entry_point);

    new_process->is_foreground = is_foreground;

    enqueue_process(process_queues[0], new_process);

    process_count++;

    return new_process->pid;
}

void free_terminated_processes(void)
{
    PCB *terminated_process = NULL;

    while ((terminated_process = (PCB *)dequeue_process(terminated_processes_queue)) != NULL)
    {
        terminated_process->state = TERMINATED;
        process_count--;

        if (terminated_process->stack_base != NULL)
        {
            memory_free(memory_manager, terminated_process->stack_base);
            terminated_process->stack_base = NULL;
        }
    }
}

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

            if (process_table[i].is_foreground)
            {
                process_table[i].father->is_foreground = 1;
                foreground_process = process_table[i].father->pid;
            }

            PCB *idle_process = &process_table[0];

            for (size_t j = 0; j < process_table[i].children_count; j++)
            {
                PCB *child = process_table[i].children[j];
                if (child != NULL && idle_process->children_count < MAX_CHILDREN)
                {
                    idle_process->children[idle_process->children_count++] = child;
                    child->father = idle_process;
                }
            }

            if (process_table[i].sem_wait_id != -1)
            {
                sem_post(process_table[i].sem_wait_id);
            }

            enqueue_process(terminated_processes_queue, &process_table[i]);
            return 0;
        }
    }

    return -1;
}

int kill_foreground_process()
{
    if (foreground_process <= 1)
    {
        return -1;
    }
    return kill_process(foreground_process);
}

pid_t get_current_pid()
{
    if (current_process == NULL)
    {
        return 0;
    }
    return current_process->pid;
}

int is_foreground(pid_t pid)
{
    return pid == foreground_process;
}

void wait_process(pid_t pid, int8_t *status)
{
    if (pid == 0)
    {
        return; 
    }

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (status != NULL)
            {
                *status = process_table[i].exit_status;
            }
            if (process_table[i].state == TERMINATED || process_table[i].sem_wait_id != -1)
            {
                return;
            }

            if (process_table[i].sem_wait_id == -1)
            {
                process_table[i].sem_wait_id = create_sem(0);
                if (process_table[i].sem_wait_id == -1)
                {
                    return;
                }
            }

            sem_wait(process_table[i].sem_wait_id);
            destroy_sem(process_table[i].sem_wait_id);
            process_table[i].sem_wait_id = -1;

            if (status != NULL)
            {
                *status = process_table[i].exit_status;
            }

            return;
        }
    }
}

uint8_t get_current_priority(void)
{
    if (current_process == NULL)
    {
        return 0; // Proceso idle
    }
    return current_process->priority;
}

int get_current_fds(uint16_t *fds)
{
    if (current_process == NULL || fds == NULL)
    {
        return -1;
    }

    fds[STDIN] = current_process->fds[STDIN];
    fds[STDOUT] = current_process->fds[STDOUT];

    return 0;
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

            if (process_table[i].state == READY)
            {
                enqueue_process(process_queues[newPriority], &process_table[i]);
            }

            return 0;
        }
    }

    return -1;
}

void set_exit_status(int8_t status)
{
    if (current_process == NULL || current_process->pid == 0)
    {
        return;
    }

    current_process->exit_status = status;
}

int block_process(pid_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (process_table[i].state == TERMINATED || process_table[i].state == BLOCKED)
            {
                return -1;
            }

            process_table[i].previous = process_table[i].state != RUNNING ? process_table[i].state : READY;
            process_table[i].state = BLOCKED;
            return 0;
        }
    }

    return -1; 
}

int wake_up_process(pid_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (process_table[i].state == TERMINATED || process_table[i].state != BLOCKED)
            {
                return -1;
            }

            if (process_table[i].previous == SLEEPING)
            {
                if (is_sleeping_time_expired(pid))
                {
                    process_table[i].state = READY;
                    enqueue_process(process_queues[process_table[i].priority], &process_table[i]);
                }
                else
                {
                    process_table[i].state = SLEEPING;
                }
            }
            else
            {
                process_table[i].state = process_table[i].previous;
                if (process_table[i].state == READY)
                {
                    enqueue_process(process_queues[process_table[i].priority], &process_table[i]);
                }
            }
            return 0;
        }
    }

    return -1;
}

int toggle_block_process(pid_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (process_table[i].state == BLOCKED)
            {
                return wake_up_process(pid);
            }
            else if (process_table[i].state != TERMINATED)
            {
                return block_process(pid);
            }
            return -1;
        }
    }

    return -1;
}

int32_t is_sleeping(pid_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            return process_table[i].state == SLEEPING;
        }
    }
    return 0;
}

int32_t sleep_process(pid_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (process_table[i].state == TERMINATED || process_table[i].state == SLEEPING)
            {
                return -1;
            }

            process_table[i].previous = process_table[i].state != RUNNING ? process_table[i].state : READY; // Me guardo su estado anterior (no guardo running)
            process_table[i].state = SLEEPING;
            return 0;
        }
    }
    return -1;
}

int32_t unsleep_process(pid_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            if (process_table[i].state == TERMINATED || process_table[i].state != SLEEPING)
            {
                return -1;
            }

            process_table[i].state = process_table[i].previous;

            if (process_table[i].state == READY)
            {
                enqueue_process(process_queues[process_table[i].priority], &process_table[i]);
            }

            return 0;
        }
    }
    return -1;
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
    return system_running && (process_count > 1); // Más que solo el proceso idle
}

int get_processes_info(ProcessInfo *process_array, int max_processes)
{
    if (process_array == NULL || max_processes <= 0)
    {
        return -1;
    }

    int count = 0;

    for (int i = 0; i < MAX_PROCESSES && count < max_processes; i++)
    {
        if (process_table[i].state != TERMINATED)
        {
            process_array[count].pid = process_table[i].pid;
            strncpy(process_array[count].name, process_table[i].name, MAX_PROCESS_NAME_LENGTH - 1);
            process_array[count].name[MAX_PROCESS_NAME_LENGTH - 1] = '\0';

            switch (process_table[i].state)
            {
            case READY:
                process_array[count].state = PS_READY;
                break;
            case RUNNING:
                process_array[count].state = PS_RUNNING;
                break;
            case BLOCKED:
                process_array[count].state = PS_BLOCKED;
                break;
            case SLEEPING:
                process_array[count].state = PS_SLEEPING;
                break;
            default:
                process_array[count].state = PS_TERMINATED;
                break;
            }

            process_array[count].priority = process_table[i].priority;
            process_array[count].parent_pid = (process_table[i].father != NULL) ? process_table[i].father->pid : 0;
            process_array[count].is_foreground = process_table[i].is_foreground;

            count++;
        }
    }

    return count;
}
