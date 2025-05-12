
#include <stdint.h>

#define NULL ((void *)0)

#define MAX_CHILDREN 10
#define MAX_PROCESSES 256

typedef struct PCB_node 
{
    PCB * head;
    PCB * tail;
} PCB_node_t;

typedef struct PCB_queue 
{
    PCB_node_t * first;
    PCB_node_t * last;
    uint8_t size;
} PCB_queue_t;

typedef enum 
{
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef struct process_control_block_t 
{
    uint64_t pid;
    char name[32];
    uint8_t priority;
    process_state_t state;
    void * stack_pointer;
    struct process_control_block_t * parent;
    struct process_control_block_t * children[MAX_CHILDREN];
    uint8_t num_children;
} PCB;

static PCB * current_process = NULL;

static PCB pcb_pool[MAX_PROCESSES] = {0};

static PCB_queue_t ready_queue = {NULL, NULL, 0};
static PCB_queue_t blocked_queue = {NULL, NULL, 0};
static PCB_queue_t terminated_queue = {NULL, NULL, 0};

static PCB * get_current_process() 
{
    return current_process;
}

static void set_current_process(PCB * process)
{
    current_process = process;
}

static PCB * get_idle_process()
{
    return &pcb_pool[0];
}

static PCB * select_next_process()
{
    if (ready_queue.size == 0) 
    {
        return NULL;
    }

    PCB * next_process = ready_queue.first->head;
    ready_queue.first = ready_queue.first->tail;
    ready_queue.size--;

    return next_process;
}

uint64_t scheduler(uint64_t current_stack_pointer)
{
    PCB * current_process = get_current_process();

    if (current_process != NULL && current_process->state == PROCESS_STATE_RUNNING)
    {
        current_process->stack_pointer = (void *)current_stack_pointer;
        current_process->state = PROCESS_STATE_READY;
        enqueue_process(current_process);
    }

    PCB * next_process = select_next_process();

    if (next_process == NULL)
    {
        next_process = get_idle_process();
    }
    next_process->state = PROCESS_STATE_RUNNING;
    set_current_process(next_process);
    return (uint64_t)next_process->stack_pointer;
}
