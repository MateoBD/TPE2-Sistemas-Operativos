#include "memory-manager.h"
#include <stdlib.h>
#include <stdio.h>
#include "test_util.h"

int main(void){
    MemoryManagerADT memory_manager;
    void * mem= malloc(MEMORY_SIZE);
    memory_manager = memory_manager_init((void *)memory_manager, mem);
    if (memory_manager == NULL) {
        printf("Memory manager initialization failed.\n");
        return 2;
    }
    printf("Memory manager initialized successfully.\n");
    test_mm(memory_manager,1000); // Test with 1 MiB of memory
    free(mem);
    return 0;
}