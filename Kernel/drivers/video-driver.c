#include <video-driver.h>

#define LEFT_MARGIN 1

static uint8_t *const video = (uint8_t *)0xB8000;
static uint8_t *current_video = (uint8_t *)0xB8000 + (LEFT_MARGIN * 2); // Start with left margin
static uint8_t current_color = 0x07; // White on black

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
    // Check if we need to scroll before drawing
    if (current_video >= video + (WIDTH * HEIGHT * 2))
    {
        vd_scroll_up();
        current_video = video + ((HEIGHT - 1) * WIDTH + LEFT_MARGIN) * 2; // Move to last line with margin
    }
    
    if (character == '\n')
    {
        NEW_LINE();
        // Check if newline caused us to go past the screen
        if (current_video >= video + (WIDTH * HEIGHT * 2))
        {
            vd_scroll_up();
            current_video = video + ((HEIGHT - 1) * WIDTH + LEFT_MARGIN) * 2; // Move to last line with margin
        }
        return;
    }
    if (character == '\b')
    {
        // Don't allow backspace beyond the left margin
        uint32_t current_col = ((current_video - video) / 2) % WIDTH;
        
        if (current_col > LEFT_MARGIN && current_video > video)
        {
            current_video -= 2;
            *(current_video) = ' ';
            *(current_video + 1) = current_color;
        }
        return;
    }
    if (character == -4)
    {
        // Handle EOF character
        // This is a no-op in the video driver, but can be used to signal end of input
        return;
    }
    
    
    // Don't allow writing beyond the right edge (leave space for margin)
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
        video[i] = ' ';        // Fill with spaces
        video[i + 1] = current_color;
    }
    current_video = video + (LEFT_MARGIN * 2); // Start at left margin
}

void vd_set_cursor(uint32_t x, uint32_t y)
{
    // Add left margin to x coordinate
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

// Scroll all lines up by one line
void vd_scroll_up(void)
{
    int i;
    
    // Move all lines up by one (copy line n+1 to line n)
    for (i = 0; i < (HEIGHT - 1) * WIDTH * 2; i++)
    {
        video[i] = video[i + (WIDTH * 2)];
    }
    
    // Clear the last line but preserve margins
    for (i = (HEIGHT - 1) * WIDTH * 2; i < HEIGHT * WIDTH * 2; i += 2)
    {
        video[i] = ' ';           // Character (space)
        video[i + 1] = current_color; // Attribute
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

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
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