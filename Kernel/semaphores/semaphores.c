#include <processes.h>
#include <stdint.h>

// Constants
#define MAX_SEMAPHORES 512
#define NOT_A_PROCESS 1025

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
} semaphore_t;

// Estructura de la cola de semáforos
typedef struct
{
    semaphore_t semaphore[MAX_SEMAPHORES];
    uint16_t current_index;
    uint16_t last_index;
} semaphore_queue_t;

// Variables de control de semáforos
static semaphore_queue_t semaphore_queue;
static uint8_t initialized = 0;

// Busca un semáforo libre en la cola
static int32_t found_free_semaphore()
{
    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        if (semaphore_queue.semaphore[i].state == SEMAPHORE_FREE)
        {
            semaphore_queue.semaphore[i].state = SEMAPHORE_USED;
            return i;
        }
    }
    return -1;
}

// Inicializa la cola de procesos en espera de un semáforo
static void initialize_semaphore_queue(uint16_t queue)
{
    for (uint16_t i = 0; i < MAX_PROCESSES; i++)
    {
        semaphore_queue.semaphore[queue].waiting_processes[i] = NOT_A_PROCESS;
    }
}

// Inicializa el gestor de semáforos
static void init_sem_manager()
{
    initialized = 1;
    semaphore_queue.current_index = 0;
    semaphore_queue.last_index = 0;
    for (int i = 0; i < MAX_SEMAPHORES; i++)
    {
        semaphore_queue.semaphore[i].state = SEMAPHORE_FREE;
        semaphore_queue.semaphore[i].value = 0;
        semaphore_queue.semaphore[i].current_index = 0;
        semaphore_queue.semaphore[i].last_index = 0;
        initialize_semaphore_queue(i);
    }
}

// Crea un nuevo semáforo con un valor inicial
int32_t create_semaphore(uint32_t initial_value)
{
    if (!initialized)
    {
        init_sem_manager();
    }

    int32_t sem = found_free_semaphore();
    if (sem == -1)
    {
        return -1;
    }

    semaphore_queue.semaphore[sem].value = initial_value;
    return sem;
}

// Realiza una operación wait en el semáforo
void semaphore_wait(uint32_t sem)
{
    if (semaphore_queue.semaphore[sem].state == SEMAPHORE_FREE || sem >= MAX_SEMAPHORES)
    {
        return; // Semáforo no encontrado o no inicializado
    }

    // Decrementar el valor del semáforo
    if (semaphore_queue.semaphore[sem].value)
    {
        semaphore_queue.semaphore[sem].value--;
    }
    else
    {
        // Agregar el proceso a la cola de espera
        pid_t current_pid = get_current_pid();
        semaphore_queue.semaphore[sem].waiting_processes[semaphore_queue.semaphore[sem].last_index] = current_pid;
        semaphore_queue.semaphore[sem].last_index++;
        semaphore_queue.semaphore[sem].last_index %= MAX_PROCESSES;
        block_process(current_pid);
    }
}

// Realiza una operación post en el semáforo
void semaphore_post(uint32_t sem)
{
    if (semaphore_queue.semaphore[sem].state == SEMAPHORE_FREE || sem >= MAX_SEMAPHORES)
    {
        return; // Semáforo no encontrado o no inicializado
    }

    // Incrementar el valor del semáforo
    semaphore_queue.semaphore[sem].value++;
    if (semaphore_queue.semaphore[sem].waiting_processes[semaphore_queue.semaphore[sem].current_index] != NOT_A_PROCESS)
    {
        // Despertar el primer proceso en espera
        pid_t pid_to_wake = semaphore_queue.semaphore[sem].waiting_processes[semaphore_queue.semaphore[sem].current_index];
        semaphore_queue.semaphore[sem].waiting_processes[semaphore_queue.semaphore[sem].current_index] = 0;
        semaphore_queue.semaphore[sem].current_index++;
        semaphore_queue.semaphore[sem].current_index %= MAX_PROCESSES;
        wake_up_process(pid_to_wake);
    }
}

// Destruye un semáforo
void destroy_semaphore(uint32_t sem)
{
    if (semaphore_queue.semaphore[sem].state == SEMAPHORE_USED)
    {
        semaphore_queue.semaphore[sem].state = SEMAPHORE_FREE;
        semaphore_queue.semaphore[sem].current_index = 0;
        semaphore_queue.semaphore[sem].last_index = 0;
        initialize_semaphore_queue(sem);
    }
}

uint32_t get_semaphore_value(uint32_t sem)
{
    if (semaphore_queue.semaphore[sem].state == SEMAPHORE_FREE || sem >= MAX_SEMAPHORES)
    {
        return 0; // Semáforo no encontrado o no inicializado
    }
    return semaphore_queue.semaphore[sem].value;
}
