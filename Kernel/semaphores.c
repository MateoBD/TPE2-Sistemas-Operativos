#include <processes.h>
#include <stdint.h>

#define MAX_SEMAPHORES 512
#define NOT_A_PROCESS 1025

enum SemaphoreState {
    SEMAPHORE_FREE = 0,
    SEMAPHORE_USED = 1
};


typedef struct {
    uint32_t value;
    pid_t waiting_processes[MAX_PROCESSES];
    uint16_t current_index;
    uint16_t last_index;
    enum SemaphoreState state;
} semaphore_t;

typedef struct {
    semaphore_t semaphore[MAX_SEMAPHORES];
    uint16_t current_index;
    uint16_t last_index;
} semaphore_queue_t;

semaphore_queue_t semaphore_queue ;
uint8_t initialized = 0;

static int32_t found_free_semaphore() {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphore_queue.semaphore[i].state == SEMAPHORE_FREE) {
            semaphore_queue.semaphore[i].state = SEMAPHORE_USED;
            return i;
        }
    }
    return -1;
}

static void initialize_semaphore_queue(uint16_t queue) {
    for (uint16_t i = 0; i < MAX_PROCESSES; i++)
    {
        semaphore_queue.semaphore[queue].waiting_processes[i] = NOT_A_PROCESS;
    }
    
}

static void initialize_semaphore_manager() {
    initialized = 1;
    semaphore_queue.current_index = 0;
    semaphore_queue.last_index = 0;
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphore_queue.semaphore[i].state = SEMAPHORE_FREE;
        semaphore_queue.semaphore[i].value = 0;
        semaphore_queue.semaphore[i].current_index = 0;
        semaphore_queue.semaphore[i].last_index = 0;
        initialize_semaphore_queue(i);
    }
}

int32_t create_semaphore(uint32_t initial_value) {
    if(!initialized) {
        initialize_semaphore_manager();
    }
    uint32_t sem;
    if((sem=found_free_semaphore())==-1) {
        return -1;
    }
    semaphore_queue.semaphore[sem].value = initial_value;
    return sem;
}

void semaphore_wait(uint32_t sem) {
    if (semaphore_queue.semaphore[sem].state == SEMAPHORE_FREE || sem >= MAX_SEMAPHORES) {
        return; // Semaphore not found or not initialized
    }

    // Decrement the semaphore value
    if(semaphore_queue.semaphore[sem].value){
        semaphore_queue.semaphore[sem].value--;
    } else {
        // Add the process to the waiting queue
        pid_t current_pid = get_current_pid(); // Assuming a function to get the current process
        semaphore_queue.semaphore[sem].waiting_processes[semaphore_queue.semaphore[sem].last_index] = current_pid;
        semaphore_queue.semaphore[sem].last_index++;
        semaphore_queue.semaphore[sem].last_index %= MAX_PROCESSES;
        block_process(current_pid);
    }
        
}

void semaphore_post(uint32_t sem) {
    if (semaphore_queue.semaphore[sem].state == SEMAPHORE_FREE || sem >= MAX_SEMAPHORES) {
        return; // Semaphore not found or not initialized
    }

    // Increment the semaphore value
    semaphore_queue.semaphore[sem].value++;
    if (semaphore_queue.semaphore[sem].waiting_processes[semaphore_queue.semaphore[sem].current_index] != NOT_A_PROCESS ) {
        // Wake up the first waiting process
        pid_t pid_to_wake = semaphore_queue.semaphore[sem].waiting_processes[semaphore_queue.semaphore[sem].current_index];
        semaphore_queue.semaphore[sem].waiting_processes[semaphore_queue.semaphore[sem].current_index] = 0;
        semaphore_queue.semaphore[sem].current_index++;
        semaphore_queue.semaphore[sem].current_index %= MAX_PROCESSES;
        wake_up_process(pid_to_wake); // Assuming a function to wake up a process
    }
}

void destroy_semaphore(uint32_t sem) {
    if (semaphore_queue.semaphore[sem].state == SEMAPHORE_USED) {
        semaphore_queue.semaphore[sem].state = SEMAPHORE_FREE;
        semaphore_queue.semaphore[sem].current_index = 0;
        semaphore_queue.semaphore[sem].last_index = 0;
        initialize_semaphore_queue(sem);
    }
}