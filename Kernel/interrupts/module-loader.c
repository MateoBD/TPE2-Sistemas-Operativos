#include <stdint.h>
#include <lib.h>
#include <module-loader.h>
#include <video-driver.h>

static void load_module(uint8_t **module, void *target_module_address);
static uint32_t read_uint_32(uint8_t **address);

void load_modules(void *payload_start, void **target_module_address)
{
    int i;
    uint8_t *current_module = (uint8_t *)payload_start;
    uint32_t module_count = read_uint_32(&current_module);

    for (i = 0; i < module_count; i++)
        load_module(&current_module, target_module_address[i]);
}

static void load_module(uint8_t **module, void *target_module_address)
{
    uint32_t module_size = read_uint_32(module);

    vd_print("  Will copy module at 0x");
    vd_print_hex((uint64_t)*module);
    vd_print(" to 0x");
    vd_print_hex((uint64_t)target_module_address);
    vd_print(" (");
    vd_print_dec(module_size);
    vd_print(" bytes)");

    memcpy(target_module_address, *module, module_size);
    *module += module_size;

    vd_print(" [Done]");
    vd_draw_char('\n');
}

static uint32_t read_uint_32(uint8_t **address)
{
    uint32_t result = *(uint32_t *)(*address);
    *address += sizeof(uint32_t);
    return result;
}
