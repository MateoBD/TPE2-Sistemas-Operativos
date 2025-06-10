#include <stdint.h>
#include <gnaio.h>
#include <test-syscalls.h>
#include <test_util.h>

#define MINOR_WAIT ((int)1E5) 
#define WAIT ((int)1E7)      

#define TOTAL_PROCESSES 3

int64_t prio[TOTAL_PROCESSES];

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;
  for (i = 0; i < TOTAL_PROCESSES; i++)
    prio[i] = i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = my_create_process("endless_loop_print", endless_loop_print, MINOR_WAIT, argv);

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_nice(pids[i], pids[TOTAL_PROCESSES - 1 - i]);

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_kill(pids[i]);
}
