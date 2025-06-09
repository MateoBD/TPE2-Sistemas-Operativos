#include <sleep-manager.h>
#include <processes.h>
#include <time.h>

typedef struct {
    pid_t pid;
    uint64_t sleep_time;
} SleepProcess;

static SleepProcess sleep_queue[MAX_PROCESSES] = {0};

static int32_t set_sleep_process(pid_t pid, uint64_t seconds)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (sleep_queue[i].pid == 0)
        {
            sleep_queue[i].pid = pid;
            sleep_queue[i].sleep_time = seconds_elapsed() + seconds;
            return i;
        }
    }
    return -1;
}

int32_t sleep(uint64_t seconds)
{
    if (seconds== 0)
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

    if (set_sleep_process(current, seconds) == -1)
    {
        return -1;
    }

    block_process(current);
    return 0;
}

void wake_up_sleeping_processes()
{
    uint64_t current_time = seconds_elapsed();

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (sleep_queue[i].pid != 0 && sleep_queue[i].sleep_time <= current_time)
        {
            pid_t pid = sleep_queue[i].pid;
            sleep_queue[i].pid = 0;
            sleep_queue[i].sleep_time = 0;

            wake_up_process(pid);
        }
    }
}
