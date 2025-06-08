#include <gnaio.h>
#include <gnalib.h>
#include <gnastring.h>
#include <test_util.h>
#include <test-syscalls.h>
#include <process-handler.h>

// Forward declarations of test functions
int64_t test_processes(uint64_t argc, char *argv[]);
uint64_t test_mm(uint64_t argc, char *argv[]);
void test_prio();
uint64_t test_sync(uint64_t argc, char *argv[]);

// Command wrapper functions that will be executed as processes
void cmd_test_processes(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: test_processes <max_processes>\n");
        printf("  max_processes: Maximum number of processes to create (positive integer)\n");
        exit(-1);
    }

    char *test_argv[1] = {argv[1]};
    int64_t result = test_processes(1, test_argv);

    if (result == -1)
    {
        printf("Error: Invalid argument for test_processes\n");
        exit(-1);
    }

    exit(0);
}

void cmd_test_mm(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: test_mm <max_memory>\n");
        printf("  max_memory: Maximum memory to allocate in bytes (positive integer)\n");
        exit(-1);
    }

    char *test_argv[1] = {argv[1]};
    uint64_t result = test_mm(1, test_argv);

    if (result == -1)
    {
        printf("Error: Invalid argument for test_mm\n");
        exit(-1);
    }

    exit(0);
}

void cmd_test_prio(int argc, char **argv)
{
    printf("Starting priority test...\n");
    test_prio();
    printf("Priority test completed.\n");
    exit(0);
}

void cmd_test_sync(int argc, char **argv)
{
    if (argc < 4)
    {
        printf("Usage: test_sync <n> <use_sem> <sem_id>\n");
        printf("  n: Number of iterations (positive integer)\n");
        printf("  use_sem: Use semaphore (0 or 1)\n");
        printf("  sem_id: Semaphore ID (string)\n");
        exit(-1);
    }

    char *test_argv[3] = {argv[1], argv[2], argv[3]};
    uint64_t result = test_sync(3, test_argv);

    if (result == -1)
    {
        printf("Error: Invalid arguments for test_sync\n");
        exit(-1);
    }

    exit(0);
}

void cmd_test_mm_basic(int argc, char **argv)
{
    // Basic memory test with default values
    char *test_argv[1] = {"1024"};
    printf("Running basic memory test with 1024 bytes...\n");
    uint64_t result = test_mm(1, test_argv);

    if (result == -1)
    {
        printf("Error: Basic memory test failed\n");
        exit(-1);
    }

    printf("Basic memory test completed successfully.\n");
    exit(0);
}

void cmd_test_help(int argc, char **argv)
{
    printf("Available test commands:\n");
    printf("  test_processes <max_processes> - Test process creation and management\n");
    printf("  test_mm <max_memory>          - Test memory manager allocation/deallocation\n");
    printf("  test_prio                     - Test priority-based scheduling\n");
    printf("  test_sync <n> <use_sem> <id>  - Test semaphore synchronization\n");
    printf("  test_mm_basic                 - Run basic memory test with default settings\n");
    printf("  test_help                     - Show this help message\n");
    printf("\nExamples:\n");
    printf("  test_processes 5              - Create up to 5 test processes\n");
    printf("  test_mm 2048                  - Test memory allocation up to 2048 bytes\n");
    printf("  test_sync 100 1 mysem         - Test sync with 100 iterations, using semaphore 'mysem'\n");
    exit(0);
}

// Public wrapper functions for shell integration
void test_processes_wrapper(int argc, char **argv)
{
    process_handler("test_processes", cmd_test_processes, argc, argv);
}

void test_mm_wrapper(int argc, char **argv)
{
    process_handler("test_mm", cmd_test_mm, argc, argv);
}

void test_prio_wrapper(int argc, char **argv)
{
    process_handler("test_prio", cmd_test_prio, argc, argv);
}

void test_sync_wrapper(int argc, char **argv)
{
    process_handler("test_sync", cmd_test_sync, argc, argv);
}

void test_mm_basic_wrapper(int argc, char **argv)
{
    process_handler("test_mm_basic", cmd_test_mm_basic, argc, argv);
}

void test_help_wrapper(int argc, char **argv)
{
    process_handler("test_help", cmd_test_help, argc, argv);
}