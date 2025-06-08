#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>

enum
{
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2
};

uint64_t sys_write(uint64_t fd, uint64_t buffer, uint64_t count, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_read(uint64_t fd, uint64_t buffer, uint64_t count, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_pipe(uint64_t pipefd, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_close(uint64_t fd, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_set_cursor(uint64_t x, uint64_t y, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_set_color(uint64_t color, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_mmap(uint64_t size, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_munmap(uint64_t addr, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_create_process(uint64_t name, uint64_t entry_point, uint64_t argc, uint64_t argv, uint64_t fds, uint64_t is_foreground);

uint64_t sys_exit(uint64_t status, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_wait(uint64_t pid, uint64_t status, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_getpid(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_kill(uint64_t pid, uint64_t signal, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_getpriority(uint64_t pid, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_setpriority(uint64_t pid, uint64_t priority, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_sched_yield(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_sleep(uint64_t seconds, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_play_sound(uint64_t frequency, uint64_t duration, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_stop_sound(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_sem_open(uint64_t value, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_sem_close(uint64_t sem_id, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_sem_wait(uint64_t sem_id, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_sem_post(uint64_t sem_id, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_sem_getvalue(uint64_t sem_id, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_mem_info(uint64_t info_struct, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_block_process(uint64_t pid, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

uint64_t sys_unblock_process(uint64_t pid, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6);

#endif // _SYSCALLS_H_