/* userCodeModule.c */

#include <shell.h>
#include <stddef.h>
#include <gnalib.h>

int main() {
    shell(1, (char *[]){"shell", NULL});

    exit(0);

    return 0;
}
