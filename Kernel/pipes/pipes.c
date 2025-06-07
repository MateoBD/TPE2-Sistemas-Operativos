#include <stdint.h>
#include <stddef.h>
#define MAX_AMOUNT_PIPES 256
#define PIPE_BUFFER_SIZE 4096

typedef struct pipe_t{
    int read_fd;
    int write_fd;
    char buffer[PIPE_BUFFER_SIZE];
    int32_t read_pos;
    int32_t write_pos;
    uint8_t available;
} pipe_t;

typedef struct pipe_manager_t {
    pipe_t pipes[MAX_AMOUNT_PIPES];
    uint32_t pipe_count;
} pipe_manager_t;

pipe_manager_t global_pipe_manager;

static int pipe_man_init=0;

void init_pipe_manager() {
    global_pipe_manager.pipe_count = 0;
    for (uint32_t i = 0; i < MAX_AMOUNT_PIPES; i++) {
        global_pipe_manager.pipes[i] = (pipe_t){
            .read_fd = -1,
            .write_fd = -1,
            .buffer = {0},
            .read_pos = 0,
            .write_pos = 0,
            .available = 0
        };
    }
    pipe_man_init = 1;
}

static pipe_t* find_available_pipe() {
    for (uint32_t i = 0; i < MAX_AMOUNT_PIPES; i++) {
        if (global_pipe_manager.pipes[i].available == 0) {
            global_pipe_manager.pipes[i].available = 1; // Mark as in use
            global_pipe_manager.pipes[i].read_fd = i; // Reset read_fd
            global_pipe_manager.pipes[i].write_fd = i; // Reset write_fd
            return &global_pipe_manager.pipes[i];
        }
    }
    return NULL; // No available pipe found
}

int create_pipe(int *read_fd, int *write_fd) {
    if (!pipe_man_init) {
        init_pipe_manager();
    }

    if (global_pipe_manager.pipe_count >= MAX_AMOUNT_PIPES) {
        return -1; 
    }

    pipe_t *new_pipe = find_available_pipe();
    if (new_pipe==NULL) {
        return -1; 
    }

    global_pipe_manager.pipe_count++;

    *read_fd = new_pipe->read_fd;
    *write_fd = new_pipe->write_fd;

    return 0;
}

int close_pipe(int fd) {
    if (!pipe_man_init) {
        return -1;
    }

    for (uint32_t i = 0; i < MAX_AMOUNT_PIPES; i++) {
        if (global_pipe_manager.pipes[i].read_fd == fd || global_pipe_manager.pipes[i].write_fd == fd) {
            global_pipe_manager.pipes[i].available = 0; // Mark pipe as available
            global_pipe_manager.pipe_count--;
            return 0;
        }
    }
    return -1; // Pipe not found
}