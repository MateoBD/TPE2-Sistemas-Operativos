#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>

enum {
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2
};

uint64_t sys_write(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_read(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_pipe(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_close(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_set_cursor(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_set_color(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_mmap(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_munmap(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_brk(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_mprotect(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_fork(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_execve(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_exit(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_wait(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_getpid(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_kill(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_getpriority(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_setpriority(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_sched_yield(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_sleep(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_play_sound(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_stop_sound(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_sem_open(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_sem_close(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_sem_unlink(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_sem_wait(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_sem_post(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_sem_getvalue(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_shm_open(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_shm_unlink(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_shm_map(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

uint64_t sys_shm_unmap(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

#endif // _SYSCALLS_H_