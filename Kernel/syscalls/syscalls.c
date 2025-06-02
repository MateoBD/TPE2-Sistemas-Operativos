#include <syscalls.h>
#include <video-driver.h>
#include <keyboard-driver.h>
#include <pc-speaker-driver.h>
#include <stdint.h>
#include <memory-manager.h>
#include <processes.h>
#include <time.h>
#include <stddef.h>

#define RED 0x0C

uint64_t sys_write(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    if (rdi < 0 || (char *)rsi == NULL || rdx <= 0)
    {
        return -1; // Invalid file descriptor or buffer
    }

    if (rdi == STDOUT)
    {
        vd_nprint((char *)rsi, (uint32_t) rdx);
    }
    else if (rdi == STDERR)
    {
        uint8_t color = vd_get_color();
        vd_set_color(RED);
        vd_print((char *)rsi);
        vd_set_color(color);
    }
    else
    {
        // Handle other file descriptors
    }
    return 0;
}

uint64_t sys_read(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    if (rdi < 0 || (char *)rsi == NULL || rdx <= 0)
    {
        return -1; // Invalid file descriptor or buffer
    }
    return 0;
}

uint64_t sys_pipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_close(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_set_cursor(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    vd_set_cursor((uint32_t)rdi, (uint32_t)rsi);
    return 0;
}

uint64_t sys_set_color(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    vd_set_color((uint8_t)rdi);
    return 0;
}

uint64_t sys_mmap(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    return (uint64_t)memory_alloc(memory_manager,rdi);
}

uint64_t sys_munmap(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    memory_free(memory_manager,(void *)rdi);
    return 0;
}

uint64_t sys_brk(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_mprotect(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

#define STACK_SIZE 0x1000 // 4KB stack size

uint64_t sys_create_process(uint64_t rip, uint64_t argc, uint64_t argv, uint64_t r10, uint64_t r8, uint64_t r9)
{

    static uint8_t p = 0;
    p = (p + 1) % 2;

    return create_process((void *) rip, p, (int) argc, (char **) argv);
}

uint64_t sys_exit(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    vd_print("Exiting process with PID: ");
    vd_print_dec(get_current_pid());
    vd_draw_char('\n');
    kill_process(get_current_pid());
    call_int_20(); // Trigger a context switch
    return 0;
}

uint64_t sys_wait(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_getpid(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    return get_current_pid();
}

uint64_t sys_kill(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_getpriority(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_setpriority(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_sched_yield(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_sleep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    sleep((int)rdi);
    return 0;
}

uint64_t sys_play_sound(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_stop_sound(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_sem_open(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_sem_close(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_sem_unlink(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_sem_wait(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_sem_post(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_sem_getvalue(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_shm_open(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_shm_unlink(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_shm_map(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_shm_unmap(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementation of sys_write
    return 0;
}

uint64_t sys_mem_info(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    
    (memory_state_get(memory_manager,(HeapState *)rdi));
    return 0;
}