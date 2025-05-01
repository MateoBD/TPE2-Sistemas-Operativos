#include <pc-speaker-driver.h>
#include <stdint.h>
#include <libasm.h>

// Play sound using built-in speaker
void play_sound(uint32_t n_frequency)
{
    uint32_t div;
    uint8_t tmp;

    // Set the PIT to the desired frequency
    div = 1193180 / n_frequency;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t)(div));
    outb(0x42, (uint8_t)(div >> 8));

    // And play the sound using the PC speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3))
    {
        outb(0x61, tmp | 3);
    }
}

// make it shut up
void stop_sound()
{
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}