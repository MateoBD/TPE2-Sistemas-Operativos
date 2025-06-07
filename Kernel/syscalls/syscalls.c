#include <syscalls.h>
#include <video-driver.h>
#include <keyboard-driver.h>
#include <pc-speaker-driver.h>
#include <stdint.h>
#include <memory-manager.h>
#include <processes.h>
#include <time.h>
#include <stddef.h>
#include <semaphores.h>
#include <pipes.h>

#define RED 0x0C

uint64_t sys_write(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    if (rdi < 0 || (char *)rsi == NULL || rdx <= 0)
    {
        return -1; // Descriptor de archivo o buffer inválido
    }

    if (rdi == STDOUT)
    {
        vd_nprint((char *)rsi, (uint32_t)rdx);
    }
    else if (rdi == STDERR)
    {
        uint8_t color = vd_get_color();
        vd_set_color(RED);
        vd_nprint((char *)rsi, (uint32_t)rdx);
        vd_set_color(color);
    }
    else
    {
        write_pipe(rdi, (int8_t *)rsi, (int)rdx);
    }
    return 0;
}

uint64_t sys_read(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    if (rdi < 0 || (int8_t *)rsi == NULL || rdx <= 0)
    {
        return -1; // Descriptor de archivo o buffer inválido
    }

    int8_t *buffer = (int8_t *)rsi;

    uint32_t bytes_read = 0;

    while (bytes_read < rdx)
    {
        int8_t c = 0;
        int result = read_pipe(rdi, &c, 1);
        if (result == -1)
        {
            return bytes_read > 0 ? bytes_read : -1; // Retornar bytes leídos o error
        }

        if (c == CHAR_INTERRUPT || c == CHAR_EOF)
        {
            buffer[bytes_read] = c;
            return bytes_read + 1;
        }
        buffer[bytes_read++] = c;
    }
    return bytes_read;
}

uint64_t sys_pipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_pipe
    return 0;
}

uint64_t sys_close(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_close
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
    return (uint64_t)memory_alloc(memory_manager, rdi);
}

uint64_t sys_munmap(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    return memory_free(memory_manager, (void *)rdi);
}

uint64_t sys_brk(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_brk
    return 0;
}

uint64_t sys_mprotect(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_mprotect
    return 0;
}

#define STACK_SIZE 0x1000 // Tamaño del stack 4KB

uint64_t sys_create_process(uint64_t name, uint64_t rip, uint64_t argc, uint64_t argv, uint64_t r8, uint64_t r9)
{

    static uint8_t p = 0;
    p = (p + 1) % 2;

    uint32_t new_pid = create_process((const char *)name, (void *)rip, p, (int)argc, (char **)argv);
    call_int_20();
    return new_pid;
}

uint64_t sys_exit(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    kill_process(get_current_pid());
    call_int_20(); // Disparar un cambio de contexto
    return 0;
}

uint64_t sys_wait(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_wait
    return 0;
}

uint64_t sys_getpid(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    return get_current_pid();
}

uint64_t sys_kill(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    kill_process((pid_t)rdi);
    call_int_20(); // Disparar un cambio de contexto
    return 0;
}

uint64_t sys_getpriority(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    return get_current_priority();
}

uint64_t sys_setpriority(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    return change_priority((pid_t)rdi, (uint8_t)rsi);
}

uint64_t sys_sched_yield(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_sched_yield
    return 0;
}

uint64_t sys_sleep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    sleep((int)rdi);
    return 0;
}

uint64_t sys_play_sound(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_play_sound
    return 0;
}

uint64_t sys_stop_sound(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_stop_sound
    return 0;
}

uint64_t sys_sem_open(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    return create_sem((uint32_t)rdi);
}

uint64_t sys_sem_close(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    destroy_sem((uint32_t)rdi);
    return 0;
}

uint64_t sys_sem_wait(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    sem_wait((uint32_t)rdi);
    return 0;
}

uint64_t sys_sem_post(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    sem_post((uint32_t)rdi);
    return 0;
}

uint64_t sys_sem_getvalue(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    return get_sem_value((uint32_t)rdi);
}

uint64_t sys_shm_open(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_shm_open
    return 0;
}

uint64_t sys_shm_unlink(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_shm_unlink
    return 0;
}

uint64_t sys_shm_map(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_shm_map
    return 0;
}

uint64_t sys_shm_unmap(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{
    // Implementación de sys_shm_unmap
    return 0;
}

uint64_t sys_mem_info(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t r9)
{

    (memory_state_get(memory_manager, (HeapState *)rdi));
    return 0;
}