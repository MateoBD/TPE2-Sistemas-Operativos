#include "gnauni.h"
#include <stdint.h>
#include <gnalib.h>
#include <programs.h>
#include <process-handler.h>

#define MAX_PHILOSOPHERS 5
#define THINKING '.'
#define EATING 'E'

// Compartidos
volatile char philosopher_states[MAX_PHILOSOPHERS];
int philosopher_count = 5;

int sems[MAX_PHILOSOPHERS];
int mutex;
uint32_t philosopher_pids[MAX_PHILOSOPHERS];

#define LEFT(i) (((i) + philosopher_count - 1) % philosopher_count)
#define RIGHT(i) (((i) + 1) % philosopher_count)

// Verifica si puede comer
void test(int i) {
    if (philosopher_states[i] == 'H' &&
        philosopher_states[LEFT(i)] != EATING &&
        philosopher_states[RIGHT(i)] != EATING) {
        philosopher_states[i] = EATING;
        sem_post(sems[i]);
    }
}

void take_forks(int i) {
    sem_wait(mutex);
    philosopher_states[i] = 'H';
    test(i);
    sem_post(mutex);
    sem_wait(sems[i]);
}

void put_forks(int i) {
    sem_wait(mutex);
    philosopher_states[i] = THINKING;
    test(LEFT(i));
    test(RIGHT(i));
    sem_post(mutex);
}

void philosopher(void *arg) {
    int id = (int)(uint64_t)arg;

    while (1) {
        sleep(1); // Pensar

        take_forks(id);

        sleep(1); // Comer

        put_forks(id);
    }
}

void print_states() {
    for (int i = 0; i < philosopher_count; i++) {
        char st = philosopher_states[i];
        write(1, (st == EATING ? "E " : ". "), 2);
    }
    write(1, "\n", 1);
}

void create_philosopher(int i) {
    sems[i] = sem_init(0);
    philosopher_states[i] = THINKING;

    philosopher_pids[i] = create_process("philosopher",philosopher,1,(char *[]){(char *)(uint64_t)i},(uint16_t[]){0, 1},0);
}

void phylo_man(int argc, char **argv) {
    mutex = sem_init(1);

    for (int i = 0; i < philosopher_count; i++) {
        create_philosopher(i);
    }

    while (1) {
        print_states();
        sleep(1); // Esperar antes de imprimir de nuevo

        // Comandos stdin
        char c;
        if (read(0, &c, 1) > 0) {
            if (c == 'a' && philosopher_count < MAX_PHILOSOPHERS) {
                int i = philosopher_count;
                philosopher_count++;
                create_philosopher(i);
            } else if (c == 'r' && philosopher_count > 1) {
                philosopher_count--;
                kill(philosopher_pids[philosopher_count]);
                sem_close(sems[philosopher_count]);
            }
        }
    }
}

void phylo(int argc, char **argv) {
    process_handler("phylo", phylo_man, argc, argv);
}