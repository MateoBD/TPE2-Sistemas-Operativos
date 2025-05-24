#include <scheduler.h>
#include <pcb-queueADT.h>
#include <stdint.h>
#include <memory-manager.h>

#define NULL ((void *)0)
#define MAX_PROCESSES 1024
#define PRIORITY_LEVELS 2
#define STACK_SIZE 0x1000 // 4KB stack size

// Estados de proceso
typedef enum
{
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

// PCB - Process Control Block
typedef struct process_control_block
{
    uint32_t pid;
    ProcessState state;
    uint8_t priority;   
    void *stack;
} PCB;

// Variables globales del scheduler
static PCB process_table[MAX_PROCESSES]; 
static PCBQueueADT process_queues[PRIORITY_LEVELS];
static PCB *current_process = NULL;
static uint32_t next_pid = 1;      
static uint32_t process_count = 1;     
static int inicialized = 0;  

extern MemoryManagerADT memory_manager;

extern void _switch_context(uint64_t *currentRSP, uint64_t nextRSP);
extern void set_process_stack(int argc, char **argv, void *stack, void *entryPoint);
extern void idle_process(); // Proceso idle


int init_scheduler()
{
    for (uint8_t i = 0; i < PRIORITY_LEVELS; i++)
    {
        process_queues[i] = new_PCBQueueADT();
        if (process_queues[i] == NULL)
        {
            return -1;
        }
    }
    
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_table[i].state = TERMINATED;
    }

    create_process(idle_process, 0, 0, NULL); // Proceso idle

    inicialized = 1;

    return 0;
}

// Crea un nuevo proceso
int create_process(void * entry_point, uint8_t priority, int argc, char ** argv)
{
    if (process_count >= MAX_PROCESSES)
    {
        return -1;
    }

    int index = -1;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == 0)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        return -1;
    }

    PCB * process = &process_table[index];
    process->pid = next_pid++;
    process->state = READY;
    process->priority = priority;



    process->stack = (void *) alloc_memory(memory_manager, STACK_SIZE);

    set_process_stack(argc, argv, process->stack, entry_point);

    enqueue_process(process_queues[priority], process);

    return process->pid;
}

void * scheduler(void * current_stack)
{
    if (!inicialized)
    {
        return current_stack;
    }
    PCB * next = NULL;

    if (current_stack == NULL || current_process == NULL)
    {
        return process_table[0].stack; // Retornar el idle process si no se estan ejecutando procesos
    }

    current_process->stack = current_stack;
    current_process->state = READY;
    enqueue_process(process_queues[current_process->priority], current_process);



    current_process = NULL;
    uint8_t p = 0; // [TO-DO] Randomizar la prioridad

    current_process = (PCB *) dequeue_process(process_queues[p]);

    if (next == NULL)
    {
        return process_table[0].stack; // Retornar el idle process si no hay ninguno en la cola
    }
    return current_process->stack;
}

// Elimina un proceso
int kill_process(uint32_t pid)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            process_table[i].pid = 0; // Marcar como libre
            process_count--;
            return 0;
        }
    }

    return -1; // Proceso no encontrado
}

// Obtiene el PID del proceso actual
uint32_t get_current_pid()
{
    if (current_process == NULL)
    {
        return 0;
    }
    return current_process->pid;
}

// Obtiene la prioridad del proceso actual
uint8_t get_current_priority()
{
    if (current_process == NULL){
        return 0;
    }
    return current_process->priority;
}

// Cambia la prioridad de un proceso
int change_priority(uint32_t pid, uint8_t new_priority)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (process_table[i].pid == pid)
        {
            process_table[i].priority = new_priority;
            return 0;
        }
    }

    return -1; // Proceso no encontrado
}


