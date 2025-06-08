#include <test-syscalls.h>
#include <stdint.h>
#include <gnaio.h>
#include <gnalib.h>
#include <gnastring.h>
#include <gnauni.h>

int64_t my_getpid() {
  return get_pid();
}

int64_t my_create_process(char *name, void * function, uint64_t argc, char *argv[]) {
  return create_process(name, function, (int)argc, argv, NULL, 0);
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
  return nice((uint32_t)pid, (uint8_t)newPrio);
}

int64_t my_kill(uint64_t pid) {
  return kill(pid);
}

int64_t my_block(uint64_t pid) {
  return block(pid);
}

int64_t my_unblock(uint64_t pid) {
  return unblock(pid);
}

int64_t my_sem_open(uint64_t initialValue) {
    return sem_init(initialValue);
}

int64_t my_sem_wait(uint32_t sem_id) {
  return sem_wait((uint32_t)sem_id);
}

int64_t my_sem_post(uint32_t sem_id) {
  return sem_post((uint32_t)sem_id);
}

int64_t my_sem_close(uint32_t sem_id) {
  return sem_close((uint32_t)sem_id);
}

int64_t my_yield() {
  return sched_yield();
}

int64_t my_wait(int64_t pid, int8_t *status) {
  return wait((uint32_t)pid, status);
}
