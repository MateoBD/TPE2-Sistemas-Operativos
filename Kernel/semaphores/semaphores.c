#include <processes.h>
#include <stdint.h>

// Constants
#define MAX_SEMAPHORES 512
#define NOT_A_PROCESS 1025

#define CHECK_INICIALIZED() if (!initialized) init_sem_manager()
#define INVALID_SEM(sem) (sem >= MAX_SEMAPHORES || sem_queue.sem[sem].state == SEMAPHORE_FREE)

// Estados de semáforo
typedef enum SemaphoreState
{
    SEMAPHORE_FREE = 0,
    SEMAPHORE_USED = 1
} SemaphoreState;

// Estructura del semáforo
typedef struct
{
    uint32_t value;
    pid_t waiting_processes[MAX_PROCESSES];
    uint16_t current_index;
    uint16_t last_index;
    SemaphoreState state;
} sem_t;

// Estructura de la cola de semáforos
typedef struct
{
    sem_t sem[MAX_SEMAPHORES];
    uint16_t current_index;
    uint16_t last_index;
} sem_queue_t;

// Variables de control de semáforos
static sem_queue_t sem_queue;
static uint8_t initialized = 0;

// Busca un semáforo libre en la cola
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

// Inicializa la cola de procesos en espera de un semáforo
static void initialize_sem_queue(uint16_t queue)
{
    for (uint16_t i = 0; i < MAX_PROCESSES; i++)
    {
        sem_queue.sem[queue].waiting_processes[i] = NOT_A_PROCESS;
    }
}

static void create_stdin_sem()
{
    // Asignar el semáforo a stdin (ID 0)
    sem_queue.sem[0].value = 0;
    sem_queue.sem[0].state = SEMAPHORE_USED;
    sem_queue.sem[0].current_index = 0;
    sem_queue.sem[0].last_index = 0;
    initialize_sem_queue(0);
}

// Inicializa el gestor de semáforos
static void init_sem_manager()
{
    initialized = 1;
    sem_queue.current_index = 0;
    sem_queue.last_index = 0;

    create_stdin_sem();

    for (int i = 1; i < MAX_SEMAPHORES; i++)
    {
        sem_queue.sem[i].state = SEMAPHORE_FREE;
        sem_queue.sem[i].value = 0;
        sem_queue.sem[i].current_index = 0;
        sem_queue.sem[i].last_index = 0;
        initialize_sem_queue(i);
    }
}

// Crea un nuevo semáforo con un valor inicial
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

// Realiza una operación wait en el semáforo
void sem_wait(uint32_t sem)
{
    CHECK_INICIALIZED();

    if (INVALID_SEM(sem))
    {
        return; // Semáforo no encontrado o no inicializado
    }

    // Decrementar el valor del semáforo
    if (sem_queue.sem[sem].value)
    {
        sem_queue.sem[sem].value--;
    }
    else
    {
        // Agregar el proceso a la cola de espera
        pid_t current_pid = get_current_pid();
        sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].last_index] = current_pid;
        sem_queue.sem[sem].last_index++;
        sem_queue.sem[sem].last_index %= MAX_PROCESSES;
        block_process(current_pid);
    }
}

// Realiza una operación post en el semáforo
void sem_post(uint32_t sem)
{
    CHECK_INICIALIZED();

    if (INVALID_SEM(sem))
    {
        return; // Semáforo no encontrado o no inicializado
    }

    // Incrementar el valor del semáforo
    sem_queue.sem[sem].value++;
    if (sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].current_index] != NOT_A_PROCESS)
    {
        // Despertar el primer proceso en espera
        pid_t pid_to_wake = sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].current_index];
        sem_queue.sem[sem].waiting_processes[sem_queue.sem[sem].current_index] = 0;
        sem_queue.sem[sem].current_index++;
        sem_queue.sem[sem].current_index %= MAX_PROCESSES;
        wake_up_process(pid_to_wake);
    }
}

// Destruye un semáforo
void destroy_sem(uint32_t sem)
{
    CHECK_INICIALIZED();

    if (sem_queue.sem[sem].state == SEMAPHORE_USED)
    {
        sem_queue.sem[sem].state = SEMAPHORE_FREE;
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
        return 0; // Semáforo no encontrado o no inicializado
    }
    return sem_queue.sem[sem].value;
}
