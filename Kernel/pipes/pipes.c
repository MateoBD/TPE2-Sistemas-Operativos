#include <stdint.h>
#include <stddef.h>
#include <semaphores.h>
#include <syscalls.h>

// Constantes
#define KERNEL_PIPES_START (STDIN + 1) // El primer pipe es stdin
#define MAX_AMOUNT_PIPES 256
#define PIPE_BUFFER_SIZE 512

#define CHECK_INITIALIZED() \
    if (!initialized)       \
    init_pipe_manager()
#define INVALID_PIPE(fd) (fd >= MAX_AMOUNT_PIPES || global_pipe_manager.pipes[fd].state == PIPE_FREE)

// Estados de pipe
typedef enum PipeState
{
    PIPE_FREE = 0,
    PIPE_USED = 1
} PipeState;

// Estructura del pipe
typedef struct
{
    int8_t buffer[PIPE_BUFFER_SIZE];
    uint16_t read_pos;
    uint16_t write_pos;
    PipeState state;
    uint32_t read_sem;
    uint32_t write_sem;
} pipe_t;

typedef struct
{
    pipe_t pipes[MAX_AMOUNT_PIPES];
    uint16_t pipe_count;
} pipe_manager_t;

static pipe_manager_t global_pipe_manager;
static uint8_t initialized = 0;

void init_pipe_manager()
{
    global_pipe_manager.pipe_count = 0;
    for (uint32_t i = 0; i < MAX_AMOUNT_PIPES; i++)
    {
        global_pipe_manager.pipes[i] = (pipe_t){
            .read_pos = 0,
            .write_pos = 0,
            .buffer = {0},
            .state = (i < KERNEL_PIPES_START) ? PIPE_USED : PIPE_FREE
        };
    }

    for (uint16_t i = 0; i < KERNEL_PIPES_START; i++)
    {
        global_pipe_manager.pipes[i].read_sem = create_sem(0);
        global_pipe_manager.pipes[i].write_sem = create_sem(PIPE_BUFFER_SIZE - 1);
    }

    initialized = 1;
}

static void clean_buffer(uint16_t fd)
{
    for (uint32_t i = 0; i < PIPE_BUFFER_SIZE; i++)
    {
        global_pipe_manager.pipes[fd].buffer[i] = 0;
    }
}

static uint16_t find_available_pipe()
{
    for (uint16_t i = STDERR + 1; i < MAX_AMOUNT_PIPES; i++) // Ignoro los primeros 3 fds
    {
        if (global_pipe_manager.pipes[i].state == PIPE_FREE)
        {
            return i;
        }
    }
    return 0;
}

int create_pipe()
{
    CHECK_INITIALIZED();

    if (global_pipe_manager.pipe_count >= MAX_AMOUNT_PIPES)
    {
        return -1;
    }

    uint16_t new_pipe = find_available_pipe();
    if (new_pipe == 0)
    {
        return -1;
    }

    global_pipe_manager.pipes[new_pipe].read_pos = 0;
    global_pipe_manager.pipes[new_pipe].write_pos = 0;
    global_pipe_manager.pipes[new_pipe].state = PIPE_USED;
    clean_buffer(new_pipe);

    global_pipe_manager.pipes[new_pipe].read_sem = create_sem(0);
    global_pipe_manager.pipes[new_pipe].write_sem = create_sem(PIPE_BUFFER_SIZE - 1);

    if (global_pipe_manager.pipes[new_pipe].read_sem == -1 || global_pipe_manager.pipes[new_pipe].write_sem == -1)
    {
        global_pipe_manager.pipes[new_pipe].state = PIPE_FREE;
        destroy_sem(global_pipe_manager.pipes[new_pipe].read_sem);
        destroy_sem(global_pipe_manager.pipes[new_pipe].write_sem);
        return -1;
    }

    global_pipe_manager.pipe_count++;

    return new_pipe;
}

int close_pipe(uint16_t fd)
{
    CHECK_INITIALIZED();

    if (fd < KERNEL_PIPES_START || INVALID_PIPE(fd))
    {
        return -1;
    }

    global_pipe_manager.pipes[fd].state = PIPE_FREE;
    global_pipe_manager.pipes[fd].read_pos = 0;
    global_pipe_manager.pipes[fd].write_pos = 0;
    global_pipe_manager.pipe_count--;

    destroy_sem(global_pipe_manager.pipes[fd].read_sem);
    destroy_sem(global_pipe_manager.pipes[fd].write_sem);

    clean_buffer(fd);

    return 0;
}

int read_pipe(uint16_t fd, int8_t *buffer, int count)
{
    CHECK_INITIALIZED();

    if (fd == STDOUT || fd == STDERR || INVALID_PIPE(fd))
    {
        return -1;
    }

    if (buffer == NULL || count <= 0)
    {
        return -1;
    }

    pipe_t *pipe = &global_pipe_manager.pipes[fd];

    int bytes_read = 0;
    while (bytes_read < count)
    {
        sem_wait(pipe->read_sem);
        buffer[bytes_read] = pipe->buffer[pipe->read_pos];
        pipe->read_pos = (pipe->read_pos + 1) % PIPE_BUFFER_SIZE;
        bytes_read++;
        sem_post(pipe->write_sem);
    }

    return bytes_read;
}

int write_pipe(int fd, const int8_t *buffer, int count)
{
    CHECK_INITIALIZED();

    if (fd == STDOUT || fd == STDERR || INVALID_PIPE(fd))
    {
        return -1;
    }

    if (buffer == NULL || count <= 0)
    {
        return -1;
    }

    pipe_t *pipe = &global_pipe_manager.pipes[fd];

    int bytes_written = 0;
    while (bytes_written < count)
    {
        sem_wait(pipe->write_sem);
        pipe->buffer[pipe->write_pos] = buffer[bytes_written];
        pipe->write_pos = (pipe->write_pos + 1) % PIPE_BUFFER_SIZE;
        bytes_written++;
        sem_post(pipe->read_sem);
    }

    return bytes_written;
}

int read_pipe_nonblocking(uint16_t fd, int8_t *buffer, int count)
{
    CHECK_INITIALIZED();

    if (fd == STDOUT || fd == STDERR || INVALID_PIPE(fd))
    {
        return -1;
    }

    if (buffer == NULL || count <= 0)
    {
        return -1;
    }

    pipe_t *pipe = &global_pipe_manager.pipes[fd];

    int bytes_read = 0;
    while (bytes_read < count)
    {
        if (get_sem_value(pipe->read_sem) <= 0)
        {
            break;
        }
        sem_wait(pipe->read_sem);
        buffer[bytes_read] = pipe->buffer[pipe->read_pos];
        pipe->read_pos = (pipe->read_pos + 1) % PIPE_BUFFER_SIZE;
        bytes_read++;
        sem_post(pipe->write_sem);
    }

    return bytes_read;
}

int write_pipe_nonblocking(int fd, const int8_t *buffer, int count)
{
    CHECK_INITIALIZED();

    if (fd == STDOUT || fd == STDERR || INVALID_PIPE(fd))
    {
        return -1;
    }

    if (buffer == NULL || count <= 0)
    {
        return -1;
    }

    pipe_t *pipe = &global_pipe_manager.pipes[fd];

    int bytes_written = 0;
    while (bytes_written < count)
    {
        if (get_sem_value(pipe->write_sem) <= 0)
        {
            break;
        }
        sem_wait(pipe->write_sem);
        pipe->buffer[pipe->write_pos] = buffer[bytes_written];
        pipe->write_pos = (pipe->write_pos + 1) % PIPE_BUFFER_SIZE;
        bytes_written++;
        sem_post(pipe->read_sem);
    }

    return bytes_written;
}
