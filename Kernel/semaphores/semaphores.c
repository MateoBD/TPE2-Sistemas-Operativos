#include <processes.h>
#include <stdint.h>

#define MAX_SEMAPHORES 512
#define NOT_A_PROCESS 1025

#define CHECK_INICIALIZED() \
    if (!initialized)       \
    init_sem_manager()
#define INVALID_SEM(sem) (sem >= MAX_SEMAPHORES || sem_queue.sem[sem].state == SEMAPHORE_FREE)

typedef enum SemaphoreState
{
    SEMAPHORE_FREE = 0,
    SEMAPHORE_USED = 1
} SemaphoreState;

typedef struct
{
    uint32_t value;
    pid_t waiting_processes[MAX_PROCESSES];
    uint16_t current_index;
    uint16_t last_index;
    SemaphoreState state;
} sem_t;

typedef struct
{
    sem_t sem[MAX_SEMAPHORES];
    uint16_t current_index;
    uint16_t last_index;
} sem_queue_t;

static sem_queue_t sem_queue;
static uint8_t initialized = 0;

static int32_t found_free_sem()
{
    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        if (sem_queue.sem[i].state == SEMAPHORE_FREE)
        {
            sem_queue.sem[i].state = SEMAPHORE_USED;
            return i;
        }
    }
    return -1;
}

static void initialize_sem_queue(uint16_t queue)
{
    for (uint16_t i = 0; i < MAX_PROCESSES; i++)
    {
        sem_queue.sem[queue].waiting_processes[i] = NOT_A_PROCESS;
    }
}

static void init_sem_manager()
{
    initialized = 1;
    sem_queue.current_index = 0;
    sem_queue.last_index = 0;

    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        sem_queue.sem[i].state = SEMAPHORE_FREE;
        sem_queue.sem[i].value = 0;
        sem_queue.sem[i].current_index = 0;
        sem_queue.sem[i].last_index = 0;
        initialize_sem_queue(i);
    }
}

int32_t create_sem(uint32_t initial_value)
{
    CHECK_INICIALIZED();

    int32_t sem = found_free_sem();
    if (sem == -1)
    {
        return -1;
    }

    sem_queue.sem[sem].value = initial_value;
    return sem;
}

void sem_wait(uint32_t sem)
{
    CHECK_INICIALIZED();

    if (INVALID_SEM(sem))
    {
        return;
    }

    while (!sem_queue.sem[sem].value)
    {
        if (INVALID_SEM(sem))
        {
            return; 
        }

        pid_t current_pid = get_current_pid();
        sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].last_index] = current_pid;
        sem_queue.sem[sem].last_index++;
        sem_queue.sem[sem].last_index %= MAX_PROCESSES;
        block_process(current_pid);
        call_int_20();

        if (INVALID_SEM(sem))
        {
            return; 
        }
    }

    if (INVALID_SEM(sem))
    {
        return;
    }

    sem_queue.sem[sem].value--;
}

void sem_post(uint32_t sem)
{
    CHECK_INICIALIZED();

    if (INVALID_SEM(sem))
    {
        return; 
    }

    sem_queue.sem[sem].value++;
    if (sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].current_index] != NOT_A_PROCESS)
    {
        pid_t pid_to_wake = sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].current_index];
        sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].current_index] = NOT_A_PROCESS;
        sem_queue.sem[sem].current_index++;
        sem_queue.sem[sem].current_index %= MAX_PROCESSES;
        wake_up_process(pid_to_wake);
    }
}

void destroy_sem(uint32_t sem)
{
    CHECK_INICIALIZED();

    if (INVALID_SEM(sem))
    {
        return; 
    }

    if (sem_queue.sem[sem].state == SEMAPHORE_USED)
    {
        while (sem_queue.sem[sem].current_index != sem_queue.sem[sem].last_index)
        {
            pid_t pid_to_wake = sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].current_index];
            if (pid_to_wake != NOT_A_PROCESS)
            {
                wake_up_process(pid_to_wake);
            }
            sem_queue.sem[sem].current_index++;
            sem_queue.sem[sem].current_index %= MAX_PROCESSES;
        }

        sem_queue.sem[sem].state = SEMAPHORE_FREE;
        sem_queue.sem[sem].value = 0;
        sem_queue.sem[sem].current_index = 0;
        sem_queue.sem[sem].last_index = 0;
        initialize_sem_queue(sem);
    }
}

uint32_t get_sem_value(uint32_t sem)
{
    CHECK_INICIALIZED();

    if (INVALID_SEM(sem))
    {
        return 0; 
    }
    return sem_queue.sem[sem].value;
}
