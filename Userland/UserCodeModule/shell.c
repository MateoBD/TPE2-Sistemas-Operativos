#include <shell.h>
#include <gnaio.h>
#include <libasm.h>
#include <test.h>
#include <gnalib.h>

void shell() {
    clean_screen();
    printf("Welcome to GNA Shell!\n");
    test_mm_fn(1);
    return;
}
