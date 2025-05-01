#include <gnaio.h>

#include <libasm.h>
#include <stdarg.h>
#include <stdint.h>
#include <gnalib.h>
#include <gnastring.h>

#define BUFFER_SIZE 5120
static char output_buffer[BUFFER_SIZE];

// Helper function to process format string
static int process_format(char *buf, uint32_t *out_size, uint32_t size, const char *format, va_list args)
{
    if (out_size)
        *out_size = 0;

    if (buf == NULL)
        size = 0;

    if (format == NULL)
        return 0;

    if (size > 0 && buf == NULL)
        return -1;

    if (size > 0 && size < 2)
        return -1;

    if (size > 0 && size < strlen(format))
        return -1;

    uint32_t count = 0;
    uint32_t i = 0;
    char c;
    char temp[64];

    while ((c = format[i++]) != 0 && (size == 0 || count < size - 1))
    {
        if (c != '%')
        {
            if (buf)
                buf[count] = c;
            count++;
        }
        else
        {
            c = format[i++];
            switch (c)
            {
            case 'd':
            {
                int num = va_arg(args, int);
                int len = itoa(num, temp, 10, 0);
                for (int j = 0; j < len && (size == 0 || count < size - 1); j++)
                {
                    if (buf)
                        buf[count] = temp[j];
                    count++;
                }
                break;
            }
            case 'x':
            {
                int num = va_arg(args, int);
                int len = itoa(num, temp, 16, 0);
                for (int j = 0; j < len && (size == 0 || count < size - 1); j++)
                {
                    if (buf)
                        buf[count] = temp[j];
                    count++;
                }
                break;
            }
            case 's':
            {
                char *str = va_arg(args, char *);
                if (str == NULL)
                    str = "(null)";
                while (*str && (size == 0 || count < size - 1))
                {
                    if (buf)
                        buf[count] = *str;
                    count++;
                    str++;
                }
                break;
            }
            case 'c':
            {
                char ch = (char)va_arg(args, int);
                if (buf)
                    buf[count] = ch;
                count++;
                break;
            }
            case '%':
            {
                if (buf)
                    buf[count] = '%';
                count++;
                break;
            }
            default:
                if (buf)
                {
                    buf[count] = '%';
                    count++;
                    if (size == 0 || count < size - 1)
                    {
                        buf[count] = c;
                        count++;
                    }
                }
                else
                {
                    count += 2;
                }
                break;
            }
        }
    }

    if (out_size)
        *out_size = count;

    if (buf && size > 0)
        buf[count < size ? count : size - 1] = '\0';
    return count;
}

int printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    uint32_t size = 0;

    int count = process_format(output_buffer, &size, BUFFER_SIZE, format, args);

    sys_call(SYS_WRITE, FD_STDOUT, (uint64_t)output_buffer, size, 0);

    va_end(args);
    return count;
}

int nprintf(uint32_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    uint32_t out_size = 0;
    int count = process_format(output_buffer, &out_size, 0, format, args);

    sys_call(SYS_WRITE, FD_STDOUT, (uint64_t)output_buffer, size, 0);

    va_end(args);
    return count;
}

int fprintf(int fd, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    uint32_t out_size = 0;

    int count = process_format(output_buffer, &out_size, BUFFER_SIZE, format, args);

    // Write to the specified file descriptor

    va_end(args);
    return count;
}

int sprintf(char *buf, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    uint32_t out_size = 0;

    int count = process_format(buf, &out_size, 0, format, args);

    va_end(args);
    return count;
}

int snprintf(char *buf, uint32_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    uint32_t out_size = 0;
    int count = process_format(buf, &out_size, size, format, args);

    va_end(args);
    return count;
}

void puts(const char *str)
{
    sys_call(SYS_WRITE, FD_STDOUT, (uint64_t)str, strlen(str), 0);
}

void putchar(char c)
{
    putc(FD_STDOUT, c);
}

int putc(int fd, char c)
{
    sys_call(SYS_WRITE, fd, (uint64_t)&c, 1, 0);
    return 0;
}

int getchar(void)
{
    // This should call the system's character input function
    int c = -1;
    if (( c = getc(FD_STDIN)) > 0)
        return c;
    return EOF;
}

int getc(int fd)
{
    char c;
    if (sys_call(SYS_READ, fd, (uint64_t)&c, 1, 0) > 0)
        return c;
    return EOF;
}

int set_cursor(uint8_t x, uint8_t y)
{
    return sys_call(SYS_SET_CURSOR, x, y, 0, 0);
}

void set_color(uint8_t fg_color, uint8_t bg_color)
{
    sys_call(SYS_SET_COLOR, fg_color, bg_color, 0, 0);
}

void clean_screen(void)
{
    set_cursor(0, 0);
    for (int i = 0; i < BUFFER_SIZE; i++)
        output_buffer[i] = 0;
    nprintf(BUFFER_SIZE, output_buffer);
    set_cursor(0, 0);
}