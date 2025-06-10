#include <gnauni.h>
#include <libasm.h>
#include <stdint.h>

uint64_t create_process(const char *name, void *function, int argc, char **argv, uint16_t *fds, char is_foreground)
{
    return sys_call((uint64_t)SYS_CREATE_PROCESS, (uint64_t)name, (uint64_t)function, (uint64_t)argc, (uint64_t)argv, (uint64_t)fds, (uint64_t)is_foreground);
}

uint32_t get_pid(void)
{
    return sys_call(SYS_GETPID, 0, 0, 0, 0, 0, 0);
}

int wait(uint32_t pid, int8_t *status)
{
    return (int)sys_call(SYS_WAIT, (uint64_t)pid, (uint64_t)status, 0, 0, 0, 0);
}

int nice(uint32_t pid, uint8_t newPriority)
{
    return (int)sys_call(SYS_SETPRIORITY, (uint64_t)pid, (uint64_t)newPriority, 0, 0, 0, 0);
}

int32_t sem_init(uint32_t initial_value)
{
    return (int32_t)sys_call(SYS_SEM_OPEN, initial_value, 0, 0, 0, 0, 0);
}

int32_t sem_close(uint32_t sem)
{
    return (int32_t)sys_call(SYS_SEM_CLOSE, sem, 0, 0, 0, 0, 0);
}

int32_t sem_wait(uint32_t sem)
{
    return (int32_t)sys_call(SYS_SEM_WAIT, sem, 0, 0, 0, 0, 0);
}

int32_t sem_post(uint32_t sem)
{
    return (int32_t)sys_call(SYS_SEM_POST, sem, 0, 0, 0, 0, 0);
}

int32_t sem_getvalue(uint32_t sem)
{
    return (int32_t)sys_call(SYS_SEM_GETVALUE, sem, 0, 0, 0, 0, 0);
}

int32_t kill(uint32_t pid)
{
    return (int32_t)sys_call(SYS_KILL, pid, 0, 0, 0, 0, 0);
}

int32_t block(uint32_t pid)
{
    return (int32_t)sys_call(SYS_BLOCK, pid, 0, 0, 0, 0, 0);
}

int32_t unblock(uint32_t pid)
{
    return (int32_t)sys_call(SYS_UNBLOCK, pid, 0, 0, 0, 0, 0);
}

int toggle_block(uint32_t pid)
{
    return (int32_t)sys_call(SYS_TOGGLE_BLOCK, pid, 0, 0, 0, 0, 0);
}

int32_t sched_yield(void)
{
    return (int32_t)sys_call(SYS_SCHED_YIELD, 0, 0, 0, 0, 0, 0);
}

int32_t get_priority(uint32_t pid)
{
    return (int32_t)sys_call(SYS_GETPRIORITY, pid, 0, 0, 0, 0, 0);
}

int32_t open()
{
    return (int32_t)sys_call(SYS_OPEN, 0, 0, 0, 0, 0, 0);
}

int32_t close(int fd)
{
    return (int32_t)sys_call(SYS_CLOSE, fd, 0, 0, 0, 0, 0);
}

int32_t read(int fd, void *buffer, uint32_t size)
{
    return (int32_t)sys_call(SYS_READ, fd, (uint64_t)buffer, size, 0, 0, 0);
}

int32_t write(int fd, const void *buffer, uint32_t size)
{
    return (int32_t)sys_call(SYS_WRITE, fd, (uint64_t)buffer, size, 0, 0, 0);
}

int32_t get_ps(ProcessInfo *buffer, uint32_t size)
{
    return (int32_t)sys_call(SYS_PS, (uint64_t)buffer, size, 0, 0, 0, 0);
}
