#include <shell.h>
#include <gnaio.h>
#include <libasm.h>
#include <gnalib.h>
#include <test.h>
#define MEM_START 0x0000000000200000
#define MEM_END 0x0000000000400000
#define MEM_SIZE (MEM_END - MEM_START)

void shell() {
    printf("Welcome to GNA Shell!\n");
    sleep(20);
    printf("starting to test memory manager...\n");
    /* char* buff=my_malloc(sizeof(char) * 2);
    printf("Addres alloc: %d\n",buff);
    buff[0]='a';
    buff[1]='b';
    printf("buff[0]: %c\n", buff[0]);
    printf("buff[1]: %c\n", buff[1]);
    my_free(buff);
    printf("Free done"); */
    test_mm(MEM_SIZE);
    
    return;
}
