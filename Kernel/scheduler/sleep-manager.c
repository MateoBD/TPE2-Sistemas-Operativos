#include <sleep-manager.h>
#include <processes.h>
#include <time.h>

typedef struct
{
    pid_t pid;
    uint64_t sleep_time;
} SleepProcess;

static SleepProcess sleep_queue[MAX_PROCESSES] = {0};

static int32_t set_sleep_process(pid_t pid, uint64_t ticks)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (sleep_queue[i].pid == 0)
        {
            sleep_queue[i].pid = pid;
            sleep_queue[i].sleep_time = get_ticks() + ticks;
            return i;
        }
    }
    return -1;
}

int32_t sleep(uint64_t seconds)
{
    if (seconds == 0)
    {
        return -1;
    }

    if (!processes_initialized())
    {
        return -1;
    }

    pid_t current = get_current_pid();
    if (current == 0)
    {
        return -1;
    }
    
    uint64_t ticks = seconds * TICKS_PER_SECOND;
    if (set_sleep_process(current, ticks) == -1)
    {
        return -1;
    }

    sleep_process(current);
    return 0;
}

void wake_up_sleeping_processes()
{
    uint64_t current_time = get_ticks();

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (sleep_queue[i].pid != 0 && sleep_queue[i].sleep_time <= current_time && is_sleeping(sleep_queue[i].pid))
        {
            pid_t pid = sleep_queue[i].pid;
            sleep_queue[i].pid = 0;
            sleep_queue[i].sleep_time = 0;

            unsleep_process(pid);
        }
    }
}

int32_t is_sleeping_time_expired(pid_t pid)
{
    uint64_t current_time = get_ticks();

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (sleep_queue[i].pid == pid)
        {
            if (sleep_queue[i].sleep_time <= current_time)
            {
                sleep_queue[i].pid = 0;
                sleep_queue[i].sleep_time = 0;
                return 1;
            }
            return 0; 
        }
    }
    return 1;
}
