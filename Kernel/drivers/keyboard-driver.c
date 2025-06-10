#include <keyboard-driver.h>
#include <interrupts.h>
#include <semaphores.h>
#include <pipes.h>
#include <syscalls.h>
#include <processes.h>

#define ESC 0x01
#define CAPSLOCK 0x3A
#define LSHIFT 0x36
#define RSHIFT 0x2A
#define LCTRL 0x1D
#define RCTRL 0x9D
#define LALT 0x38
#define RALT 0xB8
#define F_01 0x3B
#define F_10 0x44
#define F_11 0x57
#define F_12 0x58
// #define CHAR_BUFFER_DIM 64

static char capslock = 0;
static char shift = 0;
static char ctrl = 0;
static char alt = 0;

extern void save_registers();
extern char get_key();

static int not_shifted_ascii[] = {
    [0x00] = NOT_DRAWBLE, // Esc
    [0x01] = NOT_DRAWBLE, // Esc (duplicate entry)
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0x0B] = '0',
    [0x0C] = '-',
    [0x0D] = '=',
    [0x0E] = '\b',
    [0x0F] = '\t',
    [0x10] = 'q',
    [0x11] = 'w',
    [0x12] = 'e',
    [0x13] = 'r',
    [0x14] = 't',
    [0x15] = 'y',
    [0x16] = 'u',
    [0x17] = 'i',
    [0x18] = 'o',
    [0x19] = 'p',
    [0x1A] = '[',
    [0x1B] = ']',
    [0x1C] = '\n',
    [0x1D] = NOT_DRAWBLE, // left control no es imprimible
    [0x1E] = 'a',
    [0x1F] = 's',
    [0x20] = 'd',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',
    [0x27] = ';',
    [0x28] = '\'',
    [0x29] = '`',
    [0x2A] = NOT_DRAWBLE, // left shift no es imprimible
    [0x2B] = '\\',
    [0x2C] = 'z',
    [0x2D] = 'x',
    [0x2E] = 'c',
    [0x2F] = 'v',
    [0x30] = 'b',
    [0x31] = 'n',
    [0x32] = 'm',
    [0x33] = ',',
    [0x34] = '.',
    [0x35] = '/',
    [0x36] = NOT_DRAWBLE, // right shift no es imprimible
    [0x37] = NOT_DRAWBLE, // (keypad) * es imprimible
    [0x38] = NOT_DRAWBLE, // left alt no es imprimible
    [0x39] = ' ',
    [0x3A] = NOT_DRAWBLE, // CapsLock no es imprimible
    [0x3B] = NOT_DRAWBLE, // F1 no es imprimible
    [0x3C] = NOT_DRAWBLE, // F2 no es imprimible
    [0x3D] = NOT_DRAWBLE, // F3 no es imprimible
    [0x3E] = NOT_DRAWBLE, // F4 no es imprimible
    [0x3F] = NOT_DRAWBLE, // F5 no es imprimible
    [0x40] = NOT_DRAWBLE, // F6 no es imprimible
    [0x41] = NOT_DRAWBLE, // F7 no es imprimible
    [0x42] = NOT_DRAWBLE, // F8 no es imprimible
    [0x43] = NOT_DRAWBLE, // F9 no es imprimible
    [0x44] = NOT_DRAWBLE, // F10 no es imprimible
    [0x45] = NOT_DRAWBLE, // NumLock no es imprimible
    [0x46] = NOT_DRAWBLE, // ScrollLock no es imprimible
    [0x47] = NOT_DRAWBLE, // (keypad) 7
    [0x48] = NOT_DRAWBLE, // (keypad) 8
    [0x49] = NOT_DRAWBLE, // (keypad) 9
    [0x4A] = NOT_DRAWBLE, // (keypad) -
    [0x4B] = NOT_DRAWBLE, // (keypad) 4
    [0x4C] = NOT_DRAWBLE, // (keypad) 5
    [0x4D] = NOT_DRAWBLE, // (keypad) 6
    [0x4E] = NOT_DRAWBLE, // (keypad) +
    [0x4F] = NOT_DRAWBLE, // (keypad) 1
    [0x50] = NOT_DRAWBLE, // (keypad) 2
    [0x51] = NOT_DRAWBLE, // (keypad) 3
    [0x52] = NOT_DRAWBLE, // (keypad) 0
    [0x53] = NOT_DRAWBLE, // (keypad) .
    [0x54] = NOT_DRAWBLE, // (keypad) Enter no es imprimible
    [0x55] = NOT_DRAWBLE, // (keypad) / es imprimible
    [0x56] = NOT_DRAWBLE, // Right Alt no es imprimible
    [0x57] = NOT_DRAWBLE, // Home no es imprimible
    [0x58] = NOT_DRAWBLE, // Up Arrow no es imprimible
    [0x59] = NOT_DRAWBLE, // Page Up no es imprimible
    [0x5A] = NOT_DRAWBLE, // Left Arrow no es imprimible
    [0x5B] = NOT_DRAWBLE, // Right Arrow no es imprimible
    [0x5C] = NOT_DRAWBLE, // End no es imprimible
    [0x5D] = NOT_DRAWBLE, // Down Arrow no es imprimible
    [0x5E] = NOT_DRAWBLE, // Page Down no es imprimible
    [0x5F] = NOT_DRAWBLE, // Insert no es imprimible
    [0x60] = NOT_DRAWBLE, // Delete no es imprimible
    [0x61] = NOT_DRAWBLE, // F11 no es imprimible
    [0x62] = NOT_DRAWBLE, // F12 no es imprimible
    [0x63] = NOT_DRAWBLE, // F13 no es imprimible
    [0x64] = NOT_DRAWBLE, // F14 no es imprimible
    [0x65] = NOT_DRAWBLE, // F15 no es imprimible
    [0x66] = NOT_DRAWBLE, // F16 no es imprimible
    [0x67] = NOT_DRAWBLE, // F17 no es imprimible
    [0x68] = NOT_DRAWBLE, // F18 no es imprimible
    [0x69] = NOT_DRAWBLE, // F19 no es imprimible
    [0x6A] = NOT_DRAWBLE, // F20 no es imprimible
    [0x6B] = NOT_DRAWBLE, // F21 no es imprimible
    [0x6C] = NOT_DRAWBLE, // F22 no es imprimible
    [0x6D] = NOT_DRAWBLE, // F23 no es imprimible
    [0x6E] = NOT_DRAWBLE, // F24 no es imprimible
    [0x6F] = NOT_DRAWBLE, // (keypad) Enter no es imprimible
    [0x70] = NOT_DRAWBLE, // (keypad) = no es imprimible
    [0x71] = NOT_DRAWBLE, // (keypad) , no es imprimible
    [0x72] = NOT_DRAWBLE, // (keypad) ( no es imprimible
    [0x73] = NOT_DRAWBLE, // (keypad) ) no es imprimible
    [0x74] = NOT_DRAWBLE, // (keypad) $ no es imprimible
    [0x75] = NOT_DRAWBLE, // (keypad) # no es imprimible
    [0x76] = NOT_DRAWBLE, // (keypad) @ no es imprimible
    [0x77] = NOT_DRAWBLE, // (keypad) ! no es imprimible
    [0x78] = NOT_DRAWBLE, // (keypad) % no es imprimible
    [0x79] = NOT_DRAWBLE, // (keypad) ^ no es imprimible
    [0x7A] = NOT_DRAWBLE, // (keypad) & no es imprimible
    [0x7B] = NOT_DRAWBLE, // (keypad) * no es imprimible
    [0x7C] = NOT_DRAWBLE, // (keypad) - no es imprimible
    [0x7D] = NOT_DRAWBLE, // (keypad) + no es imprimible
    [0x7E] = NOT_DRAWBLE, // (keypad) / no es imprimible
    [0x7F] = NOT_DRAWBLE  // (keypad) . no es imprimible
};

static int shifted_ascii[] = {
    [0x01] = NOT_DRAWBLE, // Esc
    [0x02] = '!',
    [0x03] = '@',
    [0x04] = '#',
    [0x05] = '$',
    [0x06] = '%',
    [0x07] = '^',
    [0x08] = '&',
    [0x09] = '*',
    [0x0A] = '(',
    [0x0B] = ')',
    [0x0C] = '_',
    [0x0D] = '+',
    [0x0E] = '\b',
    [0x0F] = '\t',
    [0x10] = 'Q',
    [0x11] = 'W',
    [0x12] = 'E',
    [0x13] = 'R',
    [0x14] = 'T',
    [0x15] = 'Y',
    [0x16] = 'U',
    [0x17] = 'I',
    [0x18] = 'O',
    [0x19] = 'P',
    [0x1A] = '{',
    [0x1B] = '}',
    [0x1C] = '\n',
    [0x1D] = NOT_DRAWBLE, // left control no es imprimible
    [0x1E] = 'A',
    [0x1F] = 'S',
    [0x20] = 'D',
    [0x21] = 'F',
    [0x22] = 'G',
    [0x23] = 'H',
    [0x24] = 'J',
    [0x25] = 'K',
    [0x26] = 'L',
    [0x27] = ':',
    [0x28] = '"',
    [0x29] = '~',
    [0x2A] = NOT_DRAWBLE, // left shift no es imprimible
    [0x2B] = '|',
    [0x2C] = 'Z',
    [0x2D] = 'X',
    [0x2E] = 'C',
    [0x2F] = 'V',
    [0x30] = 'B',
    [0x31] = 'N',
    [0x32] = 'M',
    [0x33] = '<',
    [0x34] = '>',
    [0x35] = '?',
    [0x36] = NOT_DRAWBLE, // right shift no es imprimible
    [0x37] = NOT_DRAWBLE, // (keypad) * es imprimible
    [0x38] = NOT_DRAWBLE, // left alt no es imprimible
    [0x39] = ' ',
    [0x3A] = NOT_DRAWBLE, // CapsLock no es imprimible
    [0x3B] = NOT_DRAWBLE, // F1 no es imprimible
    [0x3C] = NOT_DRAWBLE, // F2 no es imprimible
    [0x3D] = NOT_DRAWBLE, // F3 no es imprimible
    [0x3E] = NOT_DRAWBLE, // F4 no es imprimible
    [0x3F] = NOT_DRAWBLE, // F5 no es imprimible
    [0x40] = NOT_DRAWBLE, // F6 no es imprimible
    [0x41] = NOT_DRAWBLE, // F7 no es imprimible
    [0x42] = NOT_DRAWBLE, // F8 no es imprimible
    [0x43] = NOT_DRAWBLE, // F9 no es imprimible
    [0x44] = NOT_DRAWBLE, // F10 no es imprimible
    [0x45] = NOT_DRAWBLE, // NumLock no es imprimible
    [0x46] = NOT_DRAWBLE, // ScrollLock no es imprimible
    [0x47] = NOT_DRAWBLE, // (keypad) 7
    [0x48] = NOT_DRAWBLE, // (keypad) 8
    [0x49] = NOT_DRAWBLE, // (keypad) 9
    [0x4A] = NOT_DRAWBLE, // (keypad) -
    [0x4B] = NOT_DRAWBLE, // (keypad) 4
    [0x4C] = NOT_DRAWBLE, // (keypad) 5
    [0x4D] = NOT_DRAWBLE, // (keypad) 6
    [0x4E] = NOT_DRAWBLE, // (keypad) +
    [0x4F] = NOT_DRAWBLE, // (keypad) 1
    [0x50] = NOT_DRAWBLE, // (keypad) 2
    [0x51] = NOT_DRAWBLE, // (keypad) 3
    [0x52] = NOT_DRAWBLE, // (keypad) 0
    [0x53] = NOT_DRAWBLE, // (keypad) .
    [0x54] = NOT_DRAWBLE, // (keypad) Enter no es imprimible
    [0x55] = NOT_DRAWBLE, // (keypad) / es imprimible
    [0x56] = NOT_DRAWBLE, // Right Alt no es imprimible
    [0x57] = NOT_DRAWBLE, // Home no es imprimible
    [0x58] = NOT_DRAWBLE, // Up Arrow no es imprimible
    [0x59] = NOT_DRAWBLE, // Page Up no es imprimible
    [0x5A] = NOT_DRAWBLE, // Left Arrow no es imprimible
    [0x5B] = NOT_DRAWBLE, // Right Arrow no es imprimible
    [0x5C] = NOT_DRAWBLE, // End no es imprimible
    [0x5D] = NOT_DRAWBLE, // Down Arrow no es imprimible
    [0x5E] = NOT_DRAWBLE, // Page Down no es imprimible
    [0x5F] = NOT_DRAWBLE, // Insert no es imprimible
    [0x60] = NOT_DRAWBLE, // Delete no es imprimible
    [0x61] = NOT_DRAWBLE, // F11 no es imprimible
    [0x62] = NOT_DRAWBLE, // F12 no es imprimible
    [0x63] = NOT_DRAWBLE, // F13 no es imprimible
    [0x64] = NOT_DRAWBLE, // F14 no es imprimible
    [0x65] = NOT_DRAWBLE, // F15 no es imprimible
    [0x66] = NOT_DRAWBLE, // F16 no es imprimible
    [0x67] = NOT_DRAWBLE, // F17 no es imprimible
    [0x68] = NOT_DRAWBLE, // F18 no es imprimible
    [0x69] = NOT_DRAWBLE, // F19 no es imprimible
    [0x6A] = NOT_DRAWBLE, // F20 no es imprimible
    [0x6B] = NOT_DRAWBLE, // F21 no es imprimible
    [0x6C] = NOT_DRAWBLE, // F22 no es imprimible
    [0x6D] = NOT_DRAWBLE, // F23 no es imprimible
    [0x6E] = NOT_DRAWBLE, // F24 no es imprimible
    [0x6F] = NOT_DRAWBLE, // (keypad) Enter no es imprimible
    [0x70] = NOT_DRAWBLE, // (keypad) = no es imprimible
    [0x71] = NOT_DRAWBLE, // (keypad) , no es imprimible
    [0x72] = NOT_DRAWBLE, // (keypad) ( no es imprimible
    [0x73] = NOT_DRAWBLE, // (keypad) ) no es imprimible
    [0x74] = NOT_DRAWBLE, // (keypad) $ no es imprimible
    [0x75] = NOT_DRAWBLE, // (keypad) # no es imprimible
    [0x76] = NOT_DRAWBLE, // (keypad) @ no es imprimible
    [0x77] = NOT_DRAWBLE, // (keypad) ! no es imprimible
    [0x78] = NOT_DRAWBLE, // (keypad) % no es imprimible
    [0x79] = NOT_DRAWBLE, // (keypad) ^ no es imprimible
    [0x7A] = NOT_DRAWBLE, // (keypad) & no es imprimible
    [0x7B] = NOT_DRAWBLE, // (keypad) * no es imprimible
    [0x7C] = NOT_DRAWBLE, // (keypad) - no es imprimible
    [0x7D] = NOT_DRAWBLE, // (keypad) + no es imprimible
    [0x7E] = NOT_DRAWBLE, // (keypad) / no es imprimible
    [0x7F] = NOT_DRAWBLE  // (keypad) . no es imprimible
};

static char kd_is_special_key(char scancode)
{
    return (scancode == LSHIFT) || (scancode == RSHIFT) ||
           (scancode == LCTRL) || (scancode == RCTRL) ||
           (scancode == LALT) || (scancode == RALT) ||
           (scancode == CAPSLOCK) || (scancode == ESC) ||
           (scancode == F_11) || (scancode == F_12) ||
           ((scancode >= F_01) && (scancode <= F_10));
}

void kd_handler()
{
    char scancode = get_key();
    char release = (scancode & 0x80) != 0; // 1 if key is released, 0 if pressed
    char key = scancode & 0x7F;

    if (key == ESC)
    {
        return;
    }

    // CAPSLOCK solo cambia en key press, no en release
    if (key == CAPSLOCK && !release)
    {
        capslock = !capslock;
        return;
    }
    else if (key == LSHIFT || key == RSHIFT)
    {
        shift = !release; // shift activo cuando no está liberado
        return;
    }
    else if (key == LCTRL || key == RCTRL)
    {
        ctrl = !release; // ctrl activo cuando no está liberado
        return;
    }
    else if (key == LALT || key == RALT)
    {
        alt = !release; // alt activo cuando no está liberado
        return;
    }
    else if (!kd_is_special_key(key) && !release)
    {
        int8_t c = -1;
        if (ctrl && shifted_ascii[(uint8_t)key] == 'C')
        {
            kill_foreground_process();
            c = CHAR_INTERRUPT; // Ctrl + C
        }
        else if (ctrl && shifted_ascii[(uint8_t)key] == 'D')
        {
            c = CHAR_EOF;
        }
        else
        {
            c = (shift ^ capslock) ? shifted_ascii[(uint8_t)key] : not_shifted_ascii[(uint8_t)key];
        }
        write_pipe_nonblocking(STDIN, &c, 1);
    }
}
