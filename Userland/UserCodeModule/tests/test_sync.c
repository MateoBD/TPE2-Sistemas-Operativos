#include <stdint.h>
#include <gnaio.h>
#include <test-syscalls.h>
#include <test_util.h>
#include <gnalib.h>

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; 

void slowInc(int64_t *p, int64_t inc) {
    uint64_t aux = *p;
    my_yield(); 
    aux += inc;
    *p = aux;
}

void my_process_inc(uint64_t argc, char *argv[]) {
    uint64_t n;
    int8_t inc;
    int8_t sem;

    if (argc != 3)
        exit(1);

    if ((n = satoi(argv[0])) <= 0)
        exit(1);
    if ((inc = satoi(argv[1])) == 0)
        exit(1);
    if ((sem = satoi(argv[2])) < 0)
        exit(1);

    uint64_t i;
    for (i = 0; i < n; i++) {
        if (sem)
            my_sem_wait(sem);
        slowInc(&global, inc);
        if (sem)
            my_sem_post(sem);
    }

    exit(0);
}

int64_t test_sync(uint64_t argc, char *argv[]) {
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    if (argc != 2)
        return 1;

    int32_t sem;
    if ((sem = satoi(argv[1])) < 0)
        return -1;
    
    if (sem)
    {
        if ((sem = my_sem_open(1)) == -1) {
            printf("test_sync: ERROR opening semaphore\n");
            return -1;
        }
    }

    if (itoa(sem, argv[1], 10, 1) == 0)
    {
        printf("test_sync: ERROR converting semaphore ID to string\n");
        return -1;
    }


    char *argvDec[] = {argv[0], "-1", argv[1], NULL};
    char *argvInc[] = {argv[0], "1", argv[1], NULL};

    global = 0;

    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] = my_create_process("my_process_inc", my_process_inc, 3, argvDec);
        pids[i + TOTAL_PAIR_PROCESSES] = my_create_process("my_process_inc", my_process_inc, 3, argvInc);
    }

    int8_t exit_value = -1;

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        my_wait(pids[i], &exit_value);
        if (exit_value != 0)
            return -1;
        exit_value = -1;
        my_wait(pids[i + TOTAL_PAIR_PROCESSES], &exit_value);
        if (exit_value != 0)
            return -1;
        exit_value = -1;
    }

    if (sem)
        my_sem_close(sem);

    printf("Final value: %d\n", global);

    return 0;
}
