#include <shell.h>
#include <gnaio.h>
#include <libasm.h>
#include <gnalib.h>
#include <unigna.h>
#include <stdint.h>

uint64_t n=0;


void test_function2(int argc, char **argv) {
    printf("Test function 2 executed!\n");
    printf("Test function 2 pid: %d\n", get_pid());
    putchar('\n');


    uint64_t i = 0;
    for (i = 0; i < 1000 * 1000; )
    {
        i++;
    }

    exit(0);

}

void test_function(int argc, char **argv) {
    printf("Test function executed!\n");
    uint32_t sem = (uint32_t) argc;
    printf("Semaphore id: %d\n\n", sem);

    sem_wait(sem);

    printf("BBBBBBB\n");

    exit(0);    
}

void shell(int argc, char **argv) {
    // sleep(16);
    printf("Welcome to GNA Shell!\n");

    putchar('\n');

    uint32_t sem = sem_init(1);
    if (sem < 0) {
        printf("Failed to create semaphore.\n");
        exit(1);
    }

    // printf("Initializing a new shell process...\n");
    // sleep(16);
    //we shoul have to check this, create_process is never calling test_function always calling a new shell or userCodeModule
    // char sem_id[5];
    // itoa(sem, sem_id, 10);


    create_process((void *)test_function, (int) sem, argv);

    sem_wait(sem);
    
    printf("AAAAAAAA\n");

    exit(0);
    
    return;
}
