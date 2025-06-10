#include <programs.h>
#include <stdint.h>
#include <stddef.h>
#include <gnaio.h>
#include <gnalib.h>
#include <gnauni.h>
#include <process-handler.h>

#define MAX_PHILOSOPHERS 13
#define MIN_PHILOSOPHERS 5
#define INVALID_PID -1
#define CMD_ADD 'a'
#define CMD_REMOVE 'r'
#define CMD_QUIT 'q'

#define RIGHT_OF(x) (((x) + 1) % philosopher_count)
#define LEFT_OF(x) (((x) + philosopher_count - 1) % philosopher_count)

typedef enum
{
    PHILO_NONE = 0,
    PHILO_EATING,
    PHILO_HUNGRY,
    PHILO_THINKING
} PhilosopherState;

static char state_display_chars[] = {'0', 'E', '.', '.'};

static char *philosopher_names[] = {"Jesus", "Mateo", "Juan", "Pedro",
                                    "Simon", "Santiago", "Jacobo", "Andres",
                                    "Tomas", "Tadeo", "Felipe", "Bartolome", "Judas"};

static PhilosopherState philosopher_states[MAX_PHILOSOPHERS];
static int16_t philosopher_pids[MAX_PHILOSOPHERS];
static int philosopher_semaphores[MAX_PHILOSOPHERS];
static int global_semaphore = -1;
static uint8_t philosopher_count = 0;

void print_philosopher_states()
{
    uint8_t has_output = 0;
    for (int i = 0; i < philosopher_count; i++)
    {
        if (philosopher_states[i] != PHILO_NONE)
        {
            has_output = 1;
            printf("%c ", state_display_chars[philosopher_states[i]]);
        }
    }
    if (has_output)
    {
        printf("\n");
    }
}

void put_down_forks(uint8_t idx)
{
    sem_wait(global_semaphore);
    philosopher_states[idx] = PHILO_THINKING;
    if (philosopher_states[LEFT_OF(idx)] == PHILO_HUNGRY &&
        philosopher_states[LEFT_OF(LEFT_OF(idx))] != PHILO_EATING)
    {
        philosopher_states[LEFT_OF(idx)] = PHILO_EATING;
        sem_post(philosopher_semaphores[LEFT_OF(idx)]);
        print_philosopher_states();
    }

    if (philosopher_states[RIGHT_OF(idx)] == PHILO_HUNGRY &&
        philosopher_states[RIGHT_OF(RIGHT_OF(idx))] != PHILO_EATING)
    {
        philosopher_states[RIGHT_OF(idx)] = PHILO_EATING;
        sem_post(philosopher_semaphores[RIGHT_OF(idx)]);
        print_philosopher_states();
    }
    sem_post(global_semaphore);
}

void pick_up_forks(uint8_t idx)
{
    sem_wait(global_semaphore);
    philosopher_states[idx] = PHILO_HUNGRY;
    if (philosopher_states[LEFT_OF(idx)] != PHILO_EATING &&
        philosopher_states[RIGHT_OF(idx)] != PHILO_EATING)
    {
        philosopher_states[idx] = PHILO_EATING;
        sem_post(philosopher_semaphores[idx]);
        print_philosopher_states();
    }
    sem_post(global_semaphore);
    sem_wait(philosopher_semaphores[idx]);
}

void philosopher_process(uint8_t argc, char *argv[])
{
    uint8_t index = atoi(argv[0]);
    my_free(argv[0]);
    my_free(argv);
    printf("%s joined the table.\n", philosopher_names[index]);
    philosopher_states[index] = PHILO_THINKING;
    while (1)
    {
        sleep(1);
        pick_up_forks(index);
        sleep(1);
        put_down_forks(index);
    }
}

int8_t create_philosopher(uint8_t idx)
{
    sem_wait(global_semaphore);

    int sem_id = sem_init(0);
    if (sem_id == -1)
    {
        sem_post(global_semaphore);
        return -1;
    }

    philosopher_semaphores[idx] = sem_id;

    char **argv = my_malloc(sizeof(char *) * 2);
    argv[0] = (char *)my_malloc(sizeof(char) * 6);
    itoa(idx, argv[0], 10, 6);
    argv[1] = NULL;

    philosopher_pids[idx] = create_process(philosopher_names[idx], philosopher_process, 1, argv, 0, 0);

    if (philosopher_pids[idx] != INVALID_PID)
    {
        philosopher_count++;
    }
    else
    {
        sem_close(sem_id);
        my_free((void *)argv[0]);
        my_free((void *)argv);
        sem_post(global_semaphore);
        return -1;
    }

    print_philosopher_states();
    sem_post(global_semaphore);
    return 0;
}

void remove_philosopher(uint8_t idx)
{
    sem_wait(global_semaphore);
    printf("%s leaves the table.\n", philosopher_names[idx]);

    while (philosopher_states[LEFT_OF(idx)] == PHILO_EATING &&
           philosopher_states[RIGHT_OF(idx)] == PHILO_EATING)
    {
        sem_post(global_semaphore);
        sem_wait(philosopher_semaphores[idx]);
        sem_wait(global_semaphore);
    }

    kill(philosopher_pids[idx]);
    int8_t status;
    wait(philosopher_pids[idx], &status);
    sem_close(philosopher_semaphores[idx]);

    philosopher_pids[idx] = INVALID_PID;
    philosopher_states[idx] = PHILO_NONE;
    philosopher_semaphores[idx] = -1;
    philosopher_count--;

    sem_post(global_semaphore);
}

void remove_all_philosophers(int max)
{
    for (int i = max - 1; i >= 0; i--)
    {
        if (philosopher_states[i] != PHILO_NONE)
        {
            remove_philosopher(i);
        }
    }
}

void run_philosophers(int argc, char *argv[])
{
    printf("Dining Philosophers Problem.\nEach philosopher needs two forks to eat.\n"
           "Press 'a' to add a philosopher, 'r' to remove one, or 'q' to quit.\n");

    global_semaphore = sem_init(1);
    if (global_semaphore == -1)
    {
        printf("Failed to initialize global semaphore.\n");
        exit(-1);
    }

    for (uint8_t i = 0; i < MAX_PHILOSOPHERS; i++)
    {
        philosopher_states[i] = PHILO_NONE;
        philosopher_pids[i] = INVALID_PID;
        philosopher_semaphores[i] = -1;
    }

    for (int i = 0; i < MIN_PHILOSOPHERS; i++)
    {
        if (create_philosopher(i) == -1)
        {
            printf("Failed to start philosopher.\n");
            remove_all_philosophers(i);
            sem_close(global_semaphore);
            exit(-1);
        }
    }

    char cmd = 0;
    while ((cmd = getchar()) != CMD_QUIT)
    {
        printf("\n");
        switch (cmd)
        {
        case CMD_ADD:
            if (philosopher_count < MAX_PHILOSOPHERS)
            {
                if (create_philosopher(philosopher_count) == -1)
                    printf("Failed to add philosopher.\n");
            }
            else
            {
                printf("Maximum number of philosophers reached.\n");
            }
            break;
        case CMD_REMOVE:
            if (philosopher_count > MIN_PHILOSOPHERS)
            {
                remove_philosopher(philosopher_count - 1);
            }
            else
            {
                printf("Minimum number of philosophers reached.\n");
            }
            break;
        }
    }

    remove_all_philosophers(philosopher_count);
    sem_close(global_semaphore);
    printf("\n");
    exit(0);
}

void phylo(int argc, char *argv[], uint16_t *fds)
{
    if (argc > 0 && argv[argc - 1] != NULL && argv[argc - 1][0] == '&' && argv[argc - 1][1] == '\0')
    {
        printf("phylo: Cannot run in background. Running in foreground instead.\n");
        argc--; 
    }
    process_handler("philo", run_philosophers, argc, argv, fds);
}