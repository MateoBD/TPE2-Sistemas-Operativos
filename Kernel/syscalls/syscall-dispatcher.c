#include <syscalls.h>
#include <stdarg.h>
#include <time.h>
#include <video-driver.h>
#include <keyboard-driver.h>
#include <stdint.h>

extern uint64_t *getRegisters();

enum enum_syscalls
{
    SYS_WRITE = 0,
    SYS_READ,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_SET_CURSOR,
    SYS_SET_COLOR,
    SYS_MMAP,
    SYS_MUNMAP,
    SYS_CREATE_PROCESS,
    SYS_EXIT,
    SYS_WAIT,
    SYS_GETPID,
    SYS_KILL,
    SYS_GETPRIORITY,
    SYS_SETPRIORITY,
    SYS_SCHED_YIELD,
    SYS_SLEEP,
    SYS_SEM_OPEN,
    SYS_SEM_CLOSE,
    SYS_SEM_WAIT,
    SYS_SEM_POST,
    SYS_SEM_GETVALUE,
    SYS_SHM_OPEN,
    SYS_SHM_UNLINK,
    SYS_SHM_MAP,
    SYS_SHM_UNMAP,
    SYS_MEM_INFO,
    SYS_BLOCK,
    SYS_UNBLOCK,
    SYS_TOGGLE_BLOCK,
    SYS_PS,
};

typedef uint64_t (*sys_call_t)(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9);

sys_call_t sys_call_arr[] =
    {
        [SYS_WRITE] = (sys_call_t)sys_write,
        [SYS_READ] = (sys_call_t)sys_read,
        [SYS_OPEN] = (sys_call_t)sys_open,
        [SYS_CLOSE] = (sys_call_t)sys_close,
        [SYS_SET_CURSOR] = (sys_call_t)sys_set_cursor,
        [SYS_SET_COLOR] = (sys_call_t)sys_set_color,
        [SYS_MMAP] = (sys_call_t)sys_mmap,
        [SYS_MUNMAP] = (sys_call_t)sys_munmap,
        [SYS_CREATE_PROCESS] = (sys_call_t)sys_create_process,
        [SYS_EXIT] = (sys_call_t)sys_exit,
        [SYS_WAIT] = (sys_call_t)sys_wait,
        [SYS_GETPID] = (sys_call_t)sys_getpid,
        [SYS_KILL] = (sys_call_t)sys_kill,
        [SYS_GETPRIORITY] = (sys_call_t)sys_getpriority,
        [SYS_SETPRIORITY] = (sys_call_t)sys_setpriority,
        [SYS_SCHED_YIELD] = (sys_call_t)sys_sched_yield,
        [SYS_SLEEP] = (sys_call_t)sys_sleep,
        [SYS_SEM_OPEN] = (sys_call_t)sys_sem_open,
        [SYS_SEM_CLOSE] = (sys_call_t)sys_sem_close,
        [SYS_SEM_WAIT] = (sys_call_t)sys_sem_wait,
        [SYS_SEM_POST] = (sys_call_t)sys_sem_post,
        [SYS_SEM_GETVALUE] = (sys_call_t)sys_sem_getvalue,
        [SYS_MEM_INFO] = (sys_call_t)sys_mem_info,
        [SYS_BLOCK] = (sys_call_t)sys_block_process,
        [SYS_UNBLOCK] = (sys_call_t)sys_unblock_process,
        [SYS_TOGGLE_BLOCK] = (sys_call_t)sys_toggle_block,
        [SYS_PS] = (sys_call_t)sys_ps,
};

uint64_t syscall_dispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    uint64_t ret;
    if (rax >= sizeof(sys_call_arr) / sizeof(sys_call_t))
    {
        return -1;
    }
    ret = sys_call_arr[rax](rdi, rsi, rdx, r10, r8, r9);

    return ret;
}
