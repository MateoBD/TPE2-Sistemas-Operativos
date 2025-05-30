#include <shell.h>
#include <gnaio.h>
#include <libasm.h>
#include <gnalib.h>

uint64_t n=0;


void test_function2(int argc, char **argv) {
    printf("Test function 2 executed!\n");
    printf("Test function 2 pid: %d\n", get_pid());
    putchar('\n');


    uint64_t i = 0;
    for (i = 0; i < 1000; )
    {
        i++;
    }

    exit(0);

    for (int i = 0; i < 10000000; )
    {
        i++;
    }
    

}

void test_function(int argc, char **argv) {
    printf("Test function executed!\n");
    printf("Test function pid: %d\n", get_pid());
    putchar('\n');


    create_process((void *)test_function2, argc, argv);

    uint64_t i = 0;
    for (i = 0; i < 1000000000; i++)
    {
        i++;
    }
    
}

void shell(int argc, char **argv) {
    // sleep(16);
    printf("Welcome to GNA Shell!\n");

    printf("Shell pid: %d\n", get_pid());

    printf("Shell argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("Shell argv[%d]: %s\n", i, argv[i]);
    }

    putchar('\n');

    // printf("Initializing a new shell process...\n");
    // sleep(16);
    //we shoul have to check this, create_process is never calling test_function always calling a new shell or userCodeModule
    
    // create_process((void *)test_function, argc, argv);

    // uint64_t i = 0;
    // for (i = 0; i < 1000000000; i++)
    // {
    //     i++;
    // }

    exit(0);
    
    return;
}
