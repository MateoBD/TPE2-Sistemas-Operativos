#include <scheduler.h>
#include <pcb-queueADT.h>
#include <stdint.h>
#include <memory-manager.h>
#include <stddef.h>
#include <video-driver.h>

#define MAX_PROCESSES 1024
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
    uint32_t pid;
    ProcessState state;
    uint8_t priority;   
    void *stack;
} PCB;

// Variables globales del scheduler
static PCB process_table[MAX_PROCESSES]; 
static PCBQueueADT process_queues[PRIORITY_LEVELS];
static PCBQueueADT terminated_processes_queue;
static PCB *current_process = &process_table[0]; // Proceso idle
static uint32_t next_pid = 1;      
static uint32_t process_count = 1;     
static int inicialized = 0;  

extern MemoryManagerADT memory_manager;

extern void _switch_context(uint64_t *currentRSP, uint64_t nextRSP);
extern void * set_process_stack(int argc, char **argv, void *stack, void *entryPoint);
extern void idle_process(); // Proceso idle


int init_scheduler()
{
    if (inicialized)
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
    
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        process_table[i].state = TERMINATED;
    }

    inicialized = 1;

    PCB * idle_process_pcb = &process_table[0];
    idle_process_pcb->pid = 0;
    idle_process_pcb->state = READY;
    idle_process_pcb->priority = 0;
    idle_process_pcb->stack = (void *) alloc_memory(memory_manager, STACK_SIZE);
    if (idle_process_pcb->stack == NULL)
    {
        return -1; // Error al asignar memoria para el proceso idle
    }

    idle_process_pcb->stack = set_process_stack(0, NULL, idle_process_pcb->stack + STACK_SIZE - 0x08, idle_process);

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

    PCB * process = &process_table[index];
    process->pid = next_pid++;
    process->state = READY;
    process->priority = priority;
    process_count++;

    process->stack = (void *) alloc_memory(memory_manager, STACK_SIZE);

    if (process->stack == NULL)
    {
        return -1;
    }

    process->stack = set_process_stack(argc, argv, process->stack + STACK_SIZE - 0x08, entry_point);

    enqueue_process(process_queues[priority], process);

    return process->pid;
}

static void free_terminated_processes(void)
{
    PCB * terminated_process = NULL;

    while ((terminated_process = (PCB *) dequeue_process(terminated_processes_queue)) != NULL)
    {
        terminated_process->state = TERMINATED;
        terminated_process->pid = 0;
        process_count--;
        // Liberar la memoria del stack del proceso terminado
        if (terminated_process->stack != NULL)
        {
            free_memory(memory_manager, terminated_process->stack);
            terminated_process->stack = NULL;
        }
    }
}

void * scheduler(void * current_stack)
{
    if (!inicialized)
    {
        return current_stack;
    }

    if (current_stack == NULL || current_process == NULL)
    {
        return process_table[0].stack; // Retornar el idle process si no se estan ejecutando procesos
    }

    free_terminated_processes();

    current_process->stack = current_stack;

    current_process->state = READY;

    if (current_process->pid != 0) // No enqueue el idle process
    {
        enqueue_process(process_queues[current_process->priority], current_process);
    }

    current_process = NULL;
    uint8_t p = 0; // [TO-DO] Randomizar la prioridad

    current_process = (PCB *) dequeue_process(process_queues[p]);

    if (current_process == NULL)
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
            process_table[i].state = TERMINATED;

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
