#include <gnaio.h>

#include <libasm.h>
#include <stdarg.h>
#include <stdint.h>
#include <gnalib.h>
#include <gnastring.h>

#define BUFFER_SIZE 5120
static char output_buffer[BUFFER_SIZE];

static int calc_amount_of_digits(int num, int base)
{
    int count = 0;
    if (num < 0)
        num = -num;

    while (num > 0)
    {
        num /= base;
        count++;
    }
    return count;
}

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
    char temp[128] = {0};

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
                int is_negative = 0;
                int num = va_arg(args, int);
                if (num < 0)
                {
                    is_negative = 1;
                    if (buf)
                        buf[count] = '-';
                    count++;
                    num = -num;
                }
                int amount_of_digits = calc_amount_of_digits(num,10);
                int len = itoa(num, temp, 10, ((amount_of_digits+is_negative)<=64) ? amount_of_digits : 64);
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
                int is_negative = 0;
                if (num < 0)
                {
                    if (buf)
                        buf[count] = '-';
                    count++;
                    num = -num;
                    is_negative = 1;
                }
                buf[count++] = '0';
                buf[count++] = 'x';
                int amount_of_digits = calc_amount_of_digits(num,10);
                int len = itoa(num, temp, 16, ((amount_of_digits+is_negative+2)<=128) ? amount_of_digits : 64);
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

    sys_call(SYS_WRITE, FD_STDOUT, (uint64_t)output_buffer, size, 0, 0, 0);

    va_end(args);
    return count;
}

int nprintf(uint32_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    uint32_t out_size = 0;
    int count = process_format(output_buffer, &out_size, 0, format, args);

    sys_call(SYS_WRITE, FD_STDOUT, (uint64_t)output_buffer, size, 0, 0, 0);

    va_end(args);
    return count;
}

int fprintf(int fd, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    uint32_t out_size = 0;

    int count = process_format(output_buffer, &out_size, BUFFER_SIZE, format, args);

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
    sys_call(SYS_WRITE, FD_STDOUT, (uint64_t)str, strlen(str), 0, 0, 0);
}

void putchar(char c)
{
    putc(FD_STDOUT, c);
}

int putc(int fd, char c)
{
    sys_call(SYS_WRITE, fd, (uint64_t)&c, 1, 0, 0, 0);
    return 0;
}

int getchar(void)
{
    return getc(FD_STDIN);
}

int getc(int fd)
{
    char c;
    if (sys_call(SYS_READ, fd, (uint64_t)&c, 1, 0, 0, 0) > 0)
    {
        if (c == CHAR_INTERRUPT || c == CHAR_EOF)
            return (int)c;
        
        return (unsigned char)c;
    }
    return EOF;
}

int set_cursor(uint8_t x, uint8_t y)
{
    return sys_call(SYS_SET_CURSOR, x, y, 0, 0, 0, 0);
}

void set_color(uint8_t fg_color, uint8_t bg_color)
{
    sys_call(SYS_SET_COLOR, bg_color*16+fg_color, 0, 0, 0, 0, 0);
}

void clean_screen(void)
{
    set_cursor(0, 0);
    for (int i = 0; i < BUFFER_SIZE; i++)
        output_buffer[i] = 0;
    nprintf(BUFFER_SIZE, output_buffer);
    set_cursor(1, 0);
}

void print_spaces(int n) {
    for (int i = 0; i < n; i++) {
        putchar(' ');
    }
}

void print_padded(const char *str, int width) {
    int len = 0;
    while (str[len]) len++;
    printf(str);
    print_spaces(width - len);
}

void print_int_padded(int num, int width) {
    char buf[12] = {0};
    int len = sprintf(buf, "%d", num);
    printf(buf);
    print_spaces(width - len);
}
