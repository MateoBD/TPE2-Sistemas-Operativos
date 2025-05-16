#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <module-loader.h>
#include <video-driver.h>
#include <idtLoader.h>
#include <keyboard-driver.h>
#include <scheduler.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t end_of_kernel_binary;
extern uint8_t end_of_kernel;

static const uint64_t page_size = 0x1000;

static void * const user_code_module_address = (void*)0x400000;
static void * const user_data_module_address = (void*)0x500000;

typedef int (*entry_point)();

void clear_bss(void *bss_address, uint64_t bss_size)
{
    memset(bss_address, 0, bss_size);
}

void *get_stack_base()
{
    return (void*)(
        (uint64_t)&end_of_kernel
        + page_size * 8              //The size of the stack itself, 32KiB
        - sizeof(uint64_t)           //Begin at the top of the stack
    );
}

void *initialize_kernel_binary()
{
    char buffer[10];

    vd_print("[x64BareBones]");
    vd_draw_char('\n');

    vd_print("CPU Vendor:");
    vd_print(cpu_vendor(buffer));
    vd_draw_char('\n');

    vd_print("[Loading modules]");
    vd_draw_char('\n');
    void *module_addresses[] = {
        user_code_module_address,
        user_data_module_address
    };

    load_modules(&end_of_kernel_binary, module_addresses);
    vd_print("[Done]");
    vd_draw_char('\n');
    vd_draw_char('\n');

    vd_print("[Initializing kernel's binary]");
    vd_draw_char('\n');

    clear_bss(&bss, &end_of_kernel - &bss);

    vd_print("  text: 0x");
    vd_print_hex((uint64_t)&text);
    vd_draw_char('\n');
    vd_print("  rodata: 0x");
    vd_print_hex((uint64_t)&rodata);
    vd_draw_char('\n');
    vd_print("  data: 0x");
    vd_print_hex((uint64_t)&data);
    vd_draw_char('\n');
    vd_print("  bss: 0x");
    vd_print_hex((uint64_t)&bss);
    vd_draw_char('\n');

    vd_print("[Done]");
    vd_draw_char('\n');
    vd_draw_char('\n');
    return get_stack_base();
}

extern void haltcpu(void);

int main() {    
    load_idt();

    // Iniciar el memory manager

    init_scheduler();

    ((entry_point)user_code_module_address)();
    haltcpu();
    return 0;
}
