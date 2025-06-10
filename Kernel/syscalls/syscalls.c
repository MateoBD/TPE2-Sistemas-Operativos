#include <syscalls.h>
#include <video-driver.h>
#include <keyboard-driver.h>
#include <stdint.h>
#include <memory-manager.h>
#include <processes.h>
#include <stddef.h>
#include <semaphores.h>
#include <pipes.h>
#include <lib.h>
#include <sleep-manager.h>

#define RED 0x0C

uint64_t sys_write(uint64_t fd, uint64_t buffer, uint64_t count, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    if (fd < 0 || (char *)buffer == NULL || count <= 0)
    {
        return -1;
    }

    uint16_t fds[2];
    get_current_fds(fds);
    fd = (fd == STDOUT) ? fds[STDOUT] : fd;

    if (fd == STDOUT)
    {
        vd_nprint((char *)buffer, (uint32_t)count);
    }
    else if (fd == STDERR)
    {
        uint8_t color = vd_get_color();
        vd_set_color(RED);
        vd_nprint((char *)buffer, (uint32_t)count);
        vd_set_color(color);
    }
    else
    {
        write_pipe(fd, (int8_t *)buffer, (int)count);
    }
    return 0;
}

uint64_t sys_read(uint64_t fd, uint64_t buffer, uint64_t count, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    if (fd < 0 || (int8_t *)buffer == NULL || count <= 0)
    {
        return -1; 
    }

    int8_t *buf = (int8_t *)buffer;

    uint32_t bytes_read = 0;

    uint16_t fds[2];
    get_current_fds(fds);
    fd = (fd == STDIN) ? fds[STDIN] : fd;

    if (fd == STDIN && !is_foreground(get_current_pid()))
    {
        return -1;
    }

    while (bytes_read < count)
    {
        int8_t c = 0;
        int result = read_pipe(fd, &c, 1);
        if (result == -1)
        {
            return bytes_read > 0 ? bytes_read : -1;
        }

        if (c == CHAR_INTERRUPT || c == CHAR_EOF)
        {
            buf[bytes_read] = c;
            return bytes_read + 1;
        }
        buf[bytes_read++] = c;
    }
    return bytes_read;
}

uint64_t sys_open(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    int fd = create_pipe();
    if (fd == -1)
    {
        return -1; 
    }
    return (uint64_t)fd;
}

uint64_t sys_close(uint64_t fd, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    if (fd == STDIN || fd == STDOUT || fd == STDERR)
    {
        return -1;
    }
    if (close_pipe((uint16_t)fd) == -1)
    {
        return -1;
    }
    return 0;
}

uint64_t sys_set_cursor(uint64_t x, uint64_t y, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    vd_set_cursor((uint32_t)x, (uint32_t)y);
    return 0;
}

uint64_t sys_set_color(uint64_t color, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    vd_set_color((uint8_t)color);
    return 0;
}

uint64_t sys_mmap(uint64_t size, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    return (uint64_t)memory_alloc(memory_manager, size);
}

uint64_t sys_munmap(uint64_t addr, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    return memory_free(memory_manager, (void *)addr);
}

uint64_t sys_create_process(uint64_t name, uint64_t entry_point, uint64_t argc, uint64_t argv, uint64_t fds, uint64_t is_foreground)
{
    uint32_t new_pid = create_process((const char *)name, (void *)entry_point, (int)argc, (char **)argv, (uint16_t *)fds, (char)is_foreground);
    if (is_foreground)
    {
        int8_t exit_status = -1;
        wait_process(new_pid, &exit_status);
        return exit_status;
    }
    return new_pid;
}

uint64_t sys_exit(uint64_t status, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    set_exit_status(status);
    kill_process(get_current_pid());
    call_int_20();
    return 0;
}

uint64_t sys_wait(uint64_t pid, uint64_t status, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    int8_t *exit_status = (int8_t *)status;

    if (pid >= MAX_PROCESSES || pid == get_current_pid())
    {
        return -1;
    }

    wait_process(pid, exit_status);

    return 0;
}

uint64_t sys_getpid(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    return get_current_pid();
}

uint64_t sys_kill(uint64_t pid, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    kill_process((pid_t)pid);
    call_int_20(); 
    return 0;
}

uint64_t sys_getpriority(uint64_t pid, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    return get_current_priority();
}

uint64_t sys_setpriority(uint64_t pid, uint64_t priority, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    return change_priority((pid_t)pid, (uint8_t)priority);
}

uint64_t sys_sched_yield(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    call_int_20();
    return 0;
}

uint64_t sys_sleep(uint64_t seconds, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    if (sleep((int)seconds) == -1)
    {
        return (uint64_t)-1;
    }
    call_int_20();
    return 0;
}

uint64_t sys_sem_open(uint64_t value, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    return create_sem((uint32_t)value);
}

uint64_t sys_sem_close(uint64_t sem_id, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    destroy_sem((uint32_t)sem_id);
    return 0;
}

uint64_t sys_sem_wait(uint64_t sem_id, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    sem_wait((uint32_t)sem_id);
    return 0;
}

uint64_t sys_sem_post(uint64_t sem_id, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    sem_post((uint32_t)sem_id);
    return 0;
}

uint64_t sys_sem_getvalue(uint64_t sem_id, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    return get_sem_value((uint32_t)sem_id);
}

uint64_t sys_mem_info(uint64_t info_struct, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{

    (memory_state_get(memory_manager, (HeapState *)info_struct));
    return 0;
}

uint64_t sys_block_process(uint64_t pid, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    if (pid >= MAX_PROCESSES || pid == get_current_pid())
    {
        return -1; 
    }
    block_process((pid_t)pid);
    return 0;
}

uint64_t sys_unblock_process(uint64_t pid, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    if (pid >= MAX_PROCESSES || pid == get_current_pid())
    {
        return -1;
    }
    wake_up_process((pid_t)pid);
    return 0;
}

uint64_t sys_toggle_block(uint64_t pid, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    if (pid >= MAX_PROCESSES || pid == get_current_pid())
    {
        return -1; 
    }

    toggle_block_process((pid_t)pid);
    return 0;
}

uint64_t sys_ps(uint64_t process_array, uint64_t max_processes, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6)
{
    if (process_array == 0 || max_processes <= 0)
    {
        return -1;
    }

    ProcessInfo *proc_array = (ProcessInfo *)process_array;
    int count = get_processes_info(proc_array, (int)max_processes);

    return (uint64_t)count;
}
