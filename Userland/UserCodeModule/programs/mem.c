#include <gnaio.h>
#include <gnalib.h>
#include <programs.h>
#include <gnauni.h>
#include <process-handler.h>

static void print_memory_state(int argc, char **argv){
    HeapState state;
    get_heap_state(&state);
    printf("Memory manager: %s\n", state.mm_type);
    printf("Total memory: %d\n", state.total_memory);
    printf("Used memory:  %d\n", state.used_memory);
    printf("Free memory:  %d\n", state.free_memory);
    exit(0);
}

void mem(int argc, char **argv, uint16_t * fds){
    process_handler("mem", print_memory_state, argc, argv, fds);
}