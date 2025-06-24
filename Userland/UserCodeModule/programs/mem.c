#include <gnaio.h>
#include <gnalib.h>
#include <programs.h>
#include <gnauni.h>
#include <process-handler.h>

void mem(int argc, char **argv, uint16_t * fds){
    HeapState state;
    get_heap_state(&state);
    printf("Memory manager: %s\n", state.mm_type);
    printf("Total memory: %d\n", state.total_memory);
    printf("Used memory:  %d\n", state.used_memory);
    printf("Free memory:  %d\n", state.free_memory);
}