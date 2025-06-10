#include <gnaio.h>
#include <gnalib.h>
#include <gnastring.h>
#include <test_util.h>
#include <test-syscalls.h>
#include <process-handler.h>

int64_t test_processes(uint64_t argc, char *argv[]);
uint64_t test_mm(uint64_t argc, char *argv[]);
void test_prio();
uint64_t test_sync(uint64_t argc, char *argv[]);

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
    if (argc < 3)
    {
        printf("Usage: test_sync <n> <use_sem>\n");
        printf("  n: Number of iterations (positive integer)\n");
        printf("  use_sem: Use semaphore (0 or 1)\n");
        exit(-1);
    }

    char *test_argv[2] = {argv[1], argv[2]};
    uint64_t result = test_sync(2, test_argv);

    if (result == -1)
    {
        printf("Error: Invalid arguments for test_sync\n");
        exit(-1);
    }

    exit(0);
}

void cmd_test_mm_basic(int argc, char **argv)
{
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
    printf("  test processes <max_processes> - Test process creation and management\n");
    printf("  test mm <max_memory>           - Test memory manager allocation/deallocation\n");
    printf("  test prio                      - Test priority-based scheduling\n");
    printf("  test sync <n> <use_sem> <id>   - Test semaphore synchronization\n");
    printf("  test mm_basic                  - Run basic memory test with default settings\n");
    printf("  test --help or -h              - Show this help message\n");
    printf("\nExamples:\n");
    printf("  test processes 5               - Create up to 5 test processes\n");
    printf("  test mm 2048                   - Test memory allocation up to 2048 bytes\n");
    printf("  test sync 100 1 mysem          - Test sync with 100 iterations, using semaphore 'mysem'\n");
    exit(0);
}

void test_processes_wrapper(int argc, char **argv, uint16_t *fds)
{
    process_handler("test_processes", cmd_test_processes, argc, argv, fds);
}

void test_mm_wrapper(int argc, char **argv, uint16_t *fds)
{
    process_handler("test_mm", cmd_test_mm, argc, argv, fds);
}

void test_prio_wrapper(int argc, char **argv, uint16_t *fds)
{
    process_handler("test_prio", cmd_test_prio, argc, argv, fds);
}

void test_sync_wrapper(int argc, char **argv, uint16_t *fds)
{
    process_handler("test_sync", cmd_test_sync, argc, argv, fds);
}

void test_mm_basic_wrapper(int argc, char **argv, uint16_t *fds)
{
    process_handler("test_mm_basic", cmd_test_mm_basic, argc, argv, fds);
}

void test_help_wrapper(int argc, char **argv, uint16_t *fds)
{
    process_handler("test_help", cmd_test_help, argc, argv, fds);
}

void test_command(int argc, char **argv, uint16_t *fds)
{
    if (argc < 2)
    {
        printf("Usage: test <test_type> [args...]\n");
        printf("Available tests:\n");
        printf("  processes [max_processes] - Test process creation and management\n");
        printf("  mm [max_memory]          - Test memory manager allocation/deallocation\n");
        printf("  prio                     - Test priority-based scheduling\n");
        printf("  sync [iterations] [use_sem] [sem_id] - Test semaphore synchronization\n");
        printf("  mm_basic                 - Run basic memory test with default settings\n");
        printf("  -h, --help               - Show this help\n");
        return;
    }

    char *test_type = argv[1];

    if (strcmp(test_type, "processes") == 0)
    {
        test_processes_wrapper(argc - 1, &argv[1], fds);
    }
    else if (strcmp(test_type, "mm") == 0)
    {
        test_mm_wrapper(argc - 1, &argv[1], fds);
    }
    else if (strcmp(test_type, "prio") == 0)
    {
        test_prio_wrapper(argc - 1, &argv[1], fds);
    }
    else if (strcmp(test_type, "sync") == 0)
    {
        test_sync_wrapper(argc - 1, &argv[1], fds);
    }
    else if (strcmp(test_type, "mm_basic") == 0)
    {
        test_mm_basic_wrapper(argc - 1, &argv[1], fds);
    }
    else if (strcmp(test_type, "-h") == 0 || strcmp(test_type, "--help") == 0)
    {
        test_help_wrapper(argc - 1, &argv[1], fds);
    }
    else
    {
        printf("Unknown test type: %s\n", test_type);
        printf("Use 'test -h' or 'test --help' to see available tests.\n");
    }
}