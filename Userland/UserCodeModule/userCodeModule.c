/* userCodeModule.c */

#include <shell.h>
#include <stddef.h>

int main() {
    shell(1, (char *[]){"shell", NULL});
    return 0;
}
