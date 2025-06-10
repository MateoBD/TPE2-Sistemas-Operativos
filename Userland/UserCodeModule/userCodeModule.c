/* userCodeModule.c */

#include <shell.h>
#include <stddef.h>
#include <gnalib.h>

void main() {
    shell(1, (char *[]){"shell", NULL});
    exit(0);
}
