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

    sem_wait(argc);
    
    for (int i = 0; i < 1000 * 1000; i++)
    {
        /* code */
    }


    

    exit(0);    
}

void shell(int argc, char **argv) {
    // sleep(16);
    printf("Welcome to GNA Shell!\n");

    putchar('\n');


    // printf("Initializing a new shell process...\n");
    // sleep(16);
    //we shoul have to check this, create_process is never calling test_function always calling a new shell or userCodeModule

    // int sem = sem_init(0);


    // create_process((void *)test_function, (int) sem, NULL);

    // sem_wait(sem);
    
    // printf("AAAAAAAA\n");

    // for (int i = 0; i < 100 * 1000; i++)
    // {
    //     putchar(' ');
    //     putchar('\b');
    //     if ((i % (10 * 1000)) == 0) {
    //         putchar('B');
    //     }
    // }

    int c;

    while ((c = getchar()) != '\n') {
        if (c >= 0)
            putchar(c);
    }

    exit(0);
    
    return;
}
