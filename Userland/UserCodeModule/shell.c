#include <shell.h>
#include <gnaio.h>
#include <libasm.h>
#include <gnalib.h>

uint64_t n=0;

void test_function(int argc, char **argv) {
    printf("Test function executed!\n");
    sleep(16);
    while (1)
    {
        /* code */
    }
    
}

void shell(int argc, char **argv) {
    sleep(16);
    printf("Welcome to GNA Shell!\n");
    printf("Initializing a new shell process...\n");
    sleep(16);

    //we shoul have to check this, create_process is never calling test_function always calling a new shell or userCodeModule
    uint64_t pid = create_process((void *)test_function, argc, argv);
    if (pid == -1) {
        printf("Failed to create shell process.\n");
    } else{
        printf("Shell process created successfully with PID: %d\n", pid);
    } 
    
    while (1) {
/*         printf("GNA> ");
        char command[256];
        int len = get_string(command, sizeof(command));
        if (len > 0) {
            command[len] = '\0'; // Null-terminate the string
            if (strcmp(command, "exit") == 0) {
                break; // Exit the shell
            } else {
                printf("Executing command: %s\n", command);
                // Here you would typically execute the command
            }
        } */
    }
    return;
}
