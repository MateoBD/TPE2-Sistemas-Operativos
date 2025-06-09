#include <programs.h>
#include <stdint.h>
#include <stddef.h>
#include <gnaio.h>
#include <gnalib.h>
#include <gnauni.h>
#include <process-handler.h>

#define MAX_PHILOSOPHERS 10
#define MIN_PHILOSOPHERS 3
#define INVALID_PID -1
#define GLOBAL_SEM_ID 100
#define CMD_ADD 'a'
#define CMD_REMOVE 'r'
#define CMD_QUIT 'q'
#define SEM_FOR_PHILOSOPHER(x) (GLOBAL_SEM_ID + (x) + 1)
#define RIGHT_OF(x) (((x) + 1) % philosopher_count)
#define LEFT_OF(x) (((x) + philosopher_count - 1) % philosopher_count)

typedef enum { PHILO_NONE = 0, PHILO_EATING, PHILO_HUNGRY, PHILO_THINKING } PhilosopherState;

static char state_display_chars[] = {'0', 'E', '.', '.'};

static char *philosopher_names[] = {"Alonso",     "Verstappen", "Senna", "Colapinto",
                                    "Schumacher", "Fangio",     "Prost", "Lauda",
                                    "Clark",      "Piquet"};

static PhilosopherState philosopher_states[MAX_PHILOSOPHERS];
static int16_t philosopher_pids[MAX_PHILOSOPHERS];
static uint8_t philosopher_count = 0;

void print_philosopher_states() {
  uint8_t has_output = 0;
  for (int i = 0; i < philosopher_count; i++) {
    if (philosopher_states[i] != PHILO_NONE) {
      has_output = 1;
      printf("%c ", state_display_chars[philosopher_states[i]]);
    }
  }
  if (has_output) {
    printf("\n");
  }
}

void put_down_forks(uint8_t idx) {
  sem_wait(GLOBAL_SEM_ID);
  philosopher_states[idx] = PHILO_THINKING;
  if (philosopher_states[LEFT_OF(idx)] == PHILO_HUNGRY &&
      philosopher_states[LEFT_OF(LEFT_OF(idx))] != PHILO_EATING) {
    philosopher_states[LEFT_OF(idx)] = PHILO_EATING;
    sem_post(SEM_FOR_PHILOSOPHER(LEFT_OF(idx)));
    print_philosopher_states();
  }

  if (philosopher_states[RIGHT_OF(idx)] == PHILO_HUNGRY &&
      philosopher_states[RIGHT_OF(RIGHT_OF(idx))] != PHILO_EATING) {
    philosopher_states[RIGHT_OF(idx)] = PHILO_EATING;
    sem_post(SEM_FOR_PHILOSOPHER(RIGHT_OF(idx)));
    print_philosopher_states();
  }
  sem_post(GLOBAL_SEM_ID);
}

void pick_up_forks(uint8_t idx) {
  sem_wait(GLOBAL_SEM_ID);
  philosopher_states[idx] = PHILO_HUNGRY;
  if (philosopher_states[LEFT_OF(idx)] != PHILO_EATING &&
      philosopher_states[RIGHT_OF(idx)] != PHILO_EATING) {
    philosopher_states[idx] = PHILO_EATING;
    sem_post(SEM_FOR_PHILOSOPHER(idx));
    print_philosopher_states();
  }
  sem_post(GLOBAL_SEM_ID);
  sem_wait(SEM_FOR_PHILOSOPHER(idx));
}

void philosopher_process(uint8_t argc, char *argv[]) {
  uint8_t index = atoi(argv[0]);
  my_free(argv[0]);
  my_free(argv);
  printf("%s joined the table.\n", philosopher_names[index]);
  philosopher_states[index] = PHILO_THINKING;
  while (1) {
    sleep(1);
    pick_up_forks(index);
    sleep(1);
    put_down_forks(index);
  }
}

int8_t create_philosopher(uint8_t idx) {
  sem_wait(GLOBAL_SEM_ID);
  if (sem_init(0) == -1) {
    return -1;
  }

  char **argv = my_malloc(sizeof(char *) * 2);
  argv[0] = "                   ";
  itoa(idx, argv[0],10,20);
  argv[1] = NULL;
  philosopher_pids[idx] = create_process(philosopher_names[idx],philosopher_process, 1, argv, 0,0);

  if (philosopher_pids[idx] != INVALID_PID) {
    philosopher_count++;
  }
  print_philosopher_states();
  sem_post(GLOBAL_SEM_ID);
  return philosopher_pids[idx] != INVALID_PID ? 0 : -1;
}

void remove_philosopher(uint8_t idx) {
  sem_wait(GLOBAL_SEM_ID);
  printf("%s leaves the table.\n", philosopher_names[idx]);
  while (philosopher_states[LEFT_OF(idx)] == PHILO_EATING &&
         philosopher_states[RIGHT_OF(idx)] == PHILO_EATING) {
    sem_post(GLOBAL_SEM_ID);
    sem_wait(SEM_FOR_PHILOSOPHER(idx));
    sem_wait(GLOBAL_SEM_ID);
  }
  kill(philosopher_pids[idx]);
  int8_t status;
  wait(philosopher_pids[idx], &status);
  sem_close(SEM_FOR_PHILOSOPHER(idx));
  philosopher_pids[idx] = INVALID_PID;
  philosopher_states[idx] = PHILO_NONE;
  philosopher_count--;
  sem_post(GLOBAL_SEM_ID);
}

void remove_all_philosophers(int max) {
  for (int i = 0; i < max; i++) {
    remove_philosopher(i);
  }
}

void run_philosophers(int argc, char *argv[]) {
  printf("Dining Philosophers Problem.\nEach philosopher needs two forks to eat.\n"
         "Press 'a' to add a philosopher, 'r' to remove one, or 'q' to quit.\n");

  if (sem_init(1) == -1) {
    printf("Failed to initialize semaphore.\n");
    exit(-1);
  }

  for (uint8_t i = 0; i < MAX_PHILOSOPHERS; i++) {
    philosopher_states[i] = PHILO_NONE;
    philosopher_pids[i] = INVALID_PID;
  }

  for (int i = 0; i < MIN_PHILOSOPHERS; i++) {
    if (create_philosopher(i) == -1) {
      printf("Failed to start philosopher.\n");
      remove_all_philosophers(i);
      exit(-1);
    }
  }

  char cmd = 0;
  while ((cmd = getchar()) != CMD_QUIT) {
    printf("\n");
    switch (cmd) {
    case CMD_ADD:
      if (philosopher_count < MAX_PHILOSOPHERS) {
        if (create_philosopher(philosopher_count) == -1)
          printf("Failed to add philosopher.\n");
      } else {
        printf("Maximum number of philosophers reached.\n");
      }
      break;
    case CMD_REMOVE:
      if (philosopher_count > MIN_PHILOSOPHERS) {
        remove_philosopher(philosopher_count - 1);
      } else {
        printf("Minimum number of philosophers reached.\n");
      }
      break;
    }
  }

  remove_all_philosophers(philosopher_count);
  sem_close(GLOBAL_SEM_ID);
  printf("\n");
  exit(0);
}

void phylo(int argc, char *argv[]) {
  process_handler("philo", run_philosophers, argc, argv);
}