#include <stdint.h>
#include <stddef.h>

// Constantes
#define MAX_AMOUNT_PIPES 256
#define PIPE_BUFFER_SIZE 4096

// Estados de pipe
typedef enum PipeState
{
    PIPE_FREE = 0,
    PIPE_USED = 1
} PipeState;

// Estructura del pipe
typedef struct
{
    int read_fd;
    int write_fd;
    char buffer[PIPE_BUFFER_SIZE];
    int32_t read_pos;
    int32_t write_pos;
    PipeState state;
} pipe_t;

// Estructura del manager de pipes
typedef struct
{
    pipe_t pipes[MAX_AMOUNT_PIPES];
    uint32_t pipe_count;
} pipe_manager_t;

// Variables de control de pipes
static pipe_manager_t global_pipe_manager;
static uint8_t initialized = 0;

// Inicializa el manager de pipes
void init_pipe_manager()
{
    global_pipe_manager.pipe_count = 0;
    for (uint32_t i = 0; i < MAX_AMOUNT_PIPES; i++)
    {
        global_pipe_manager.pipes[i] = (pipe_t){
            .read_fd = -1,
            .write_fd = -1,
            .buffer = {0},
            .read_pos = 0,
            .write_pos = 0,
            .state = PIPE_FREE};
    }
    initialized = 1;
}

// Busca un pipe libre en el array
static pipe_t *find_available_pipe()
{
    for (uint32_t i = 0; i < MAX_AMOUNT_PIPES; i++)
    {
        if (global_pipe_manager.pipes[i].state == PIPE_FREE)
        {
            global_pipe_manager.pipes[i].state = PIPE_USED; // Mark as in use
            global_pipe_manager.pipes[i].read_fd = i;       // Reset read_fd
            global_pipe_manager.pipes[i].write_fd = i;      // Reset write_fd
            return &global_pipe_manager.pipes[i];
        }
    }
    return NULL; // No available pipe found
}

// Crea un nuevo pipe y asigna file descriptors
int create_pipe(int *read_fd, int *write_fd)
{
    if (!initialized)
    {
        init_pipe_manager();
    }

    if (global_pipe_manager.pipe_count >= MAX_AMOUNT_PIPES)
    {
        return -1;
    }

    pipe_t *new_pipe = find_available_pipe();
    if (new_pipe == NULL)
    {
        return -1;
    }

    global_pipe_manager.pipe_count++;

    *read_fd = new_pipe->read_fd;
    *write_fd = new_pipe->write_fd;

    return 0;
}

// Cierra un pipe dado su file descriptor
int close_pipe(int fd)
{
    if (!initialized)
    {
        return -1;
    }

    for (uint32_t i = 0; i < MAX_AMOUNT_PIPES; i++)
    {
        if (global_pipe_manager.pipes[i].read_fd == fd || global_pipe_manager.pipes[i].write_fd == fd)
        {
            global_pipe_manager.pipes[i].state = PIPE_FREE; // Mark pipe as available
            global_pipe_manager.pipe_count--;
            return 0;
        }
    }
    return -1; // Pipe not found
}

// Busca un pipe por su file descriptor
static pipe_t *find_pipe_by_fd(int fd)
{
    for (uint32_t i = 0; i < MAX_AMOUNT_PIPES; i++)
    {
        if (global_pipe_manager.pipes[i].state == PIPE_USED &&
            (global_pipe_manager.pipes[i].read_fd == fd || global_pipe_manager.pipes[i].write_fd == fd))
        {
            return &global_pipe_manager.pipes[i];
        }
    }
    return NULL;
}

// Lee datos del pipe
int read_pipe(int fd, char *buffer, int count)
{
    if (!initialized)
    {
        return -1;
    }

    if (buffer == NULL || count <= 0)
    {
        return -1;
    }

    pipe_t *pipe = find_pipe_by_fd(fd);
    if (pipe == NULL || pipe->read_fd != fd)
    {
        return -1; // Invalid fd or not a read fd
    }

    int bytes_read = 0;
    while (bytes_read < count && pipe->read_pos != pipe->write_pos)
    {
        buffer[bytes_read] = pipe->buffer[pipe->read_pos];
        pipe->read_pos = (pipe->read_pos + 1) % PIPE_BUFFER_SIZE;
        bytes_read++;
    }

    return bytes_read;
}

// Escribe datos al pipe
int write_pipe(int fd, const char *buffer, int count)
{
    if (!initialized)
    {
        return -1;
    }

    if (buffer == NULL || count <= 0)
    {
        return -1;
    }

    pipe_t *pipe = find_pipe_by_fd(fd);
    if (pipe == NULL || pipe->write_fd != fd)
    {
        return -1; // Invalid fd or not a write fd
    }

    int bytes_written = 0;
    while (bytes_written < count)
    {
        int next_write_pos = (pipe->write_pos + 1) % PIPE_BUFFER_SIZE;

        // Check if buffer is full
        if (next_write_pos == pipe->read_pos)
        {
            break; // Buffer full, can't write more
        }

        pipe->buffer[pipe->write_pos] = buffer[bytes_written];
        pipe->write_pos = next_write_pos;
        bytes_written++;
    }

    return bytes_written;
}