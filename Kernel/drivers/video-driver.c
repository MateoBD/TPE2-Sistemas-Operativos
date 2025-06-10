#include <video-driver.h>
#include <keyboard-driver.h>

#define LEFT_MARGIN 1

static uint8_t *const video = (uint8_t *)0xB8000;
static uint8_t *current_video = (uint8_t *)0xB8000 + (LEFT_MARGIN * 2); 
static uint8_t current_color = 0x07; // Fondo negro, letra blanca

#define NEW_LINE() do { \
    uint32_t current_line = ((current_video - video) / 2) / WIDTH; \
    current_video = video + ((current_line + 1) * WIDTH + LEFT_MARGIN) * 2; \
} while(0)

static uint32_t uint_to_base(uint64_t value, char * buffer, uint32_t base);
static char buffer[64] = { '0' };

void vd_print(const char *string)
{
    int i;

    for (i = 0; string[i] != 0; i++)
    {
        if (string[i] == '\n')
        {
            NEW_LINE();
            continue;
        }
        vd_draw_char(string[i]);
    }
}

void vd_nprint(const char *string, uint32_t n)  
{
    int i;

    for (i = 0; i < n; i++){
        vd_draw_char(string[i]);
    }
}

void vd_draw_char(char character)
{
    if (current_video >= video + (WIDTH * HEIGHT * 2))
    {
        vd_scroll_up();
        current_video = video + ((HEIGHT - 1) * WIDTH + LEFT_MARGIN) * 2;
    }
    
    if (character == '\n')
    {
        NEW_LINE();
        if (current_video >= video + (WIDTH * HEIGHT * 2))
        {
            vd_scroll_up();
            current_video = video + ((HEIGHT - 1) * WIDTH + LEFT_MARGIN) * 2;
        }
        return;
    }
    if (character == '\b')
    {
        uint32_t current_col = ((current_video - video) / 2) % WIDTH;
        
        if (current_col > LEFT_MARGIN && current_video > video)
        {
            current_video -= 2;
            *(current_video) = ' ';
            *(current_video + 1) = current_color;
        }
        return;
    }
    if (character == CHAR_EOF)
    {
        return;
    }
    
    uint32_t current_col = ((current_video - video) / 2) % WIDTH;
    if (current_col >= WIDTH - 1)
    {
        NEW_LINE();
        if (current_video >= video + (WIDTH * HEIGHT * 2))
        {
            vd_scroll_up();
            current_video = video + ((HEIGHT - 1) * WIDTH + LEFT_MARGIN) * 2;
        }
    }
    
    *current_video = character;
    *(current_video + 1) = current_color;
    current_video += 2;
}

void vd_clear_screen()
{
    int i;

    for (i = 0; i < HEIGHT * WIDTH * 2; i += 2)
    {
        video[i] = ' ';       
        video[i + 1] = current_color;
    }
    current_video = video + (LEFT_MARGIN * 2); 
}

void vd_set_cursor(uint32_t x, uint32_t y)
{
    current_video = video + ((x + LEFT_MARGIN) + (y * WIDTH)) * 2;
}

uint8_t vd_get_color()
{
    return current_color;
}

void vd_set_color(uint8_t new_color)
{
    current_color = new_color;
}

void vd_scroll_up(void)
{
    int i;
    for (i = 0; i < (HEIGHT - 1) * WIDTH * 2; i++)
    {
        video[i] = video[i + (WIDTH * 2)];
    }

    for (i = (HEIGHT - 1) * WIDTH * 2; i < HEIGHT * WIDTH * 2; i += 2)
    {
        video[i] = ' ';          
        video[i + 1] = current_color;
    }
}

void vd_print_dec(uint64_t value)
{
	vd_print_base(value, 10);
}

void vd_print_hex(uint64_t value)
{
	vd_print_base(value, 16);
}

void vd_print_bin(uint64_t value)
{
	vd_print_base(value, 2);
}

void vd_print_base(uint64_t value, uint32_t base)
{
    uint_to_base(value, buffer, base);
    vd_print(buffer);
}

static uint32_t uint_to_base(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	*p = 0;

	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}