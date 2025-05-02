#include <gnastring.h>

int strlen(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}

void strcpy(char *dest, const char *src)
{
    int i = 0;
    while ((dest[i] = src[i]) != '\0')
    {
        i++;
    }
}

void strncpy(char *dest, const char *src, int n)
{
    int i;

    for (i = 0; i < n && src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }

    for (; i < n; i++)
    {
        dest[i] = '\0';
    }
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, int n)
{
    if (n == 0)
    {
        return 0;
    }

    while (n-- > 0 && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }

    return (n < 0) ? 0 : *(unsigned char *)s1 - *(unsigned char *)s2;
}

int tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c + ('a' - 'A');
    }
    return c;
}

int toupper(int c)
{
    if (c >= 'a' && c <= 'z')
    {
        return c - ('a' - 'A');
    }
    return c;
}

int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && (tolower(*s1) == tolower(*s2)))
    {
        s1++;
        s2++;
    }
    return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}

int strncasecmp(const char *s1, const char *s2, int n)
{
    if (n == 0)
    {
        return 0;
    }

    while (n-- > 0 && *s1 && (tolower(*s1) == tolower(*s2)))
    {
        s1++;
        s2++;
    }

    return (n < 0) ? 0 : tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}

int isalpha(int c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

int isdigit(int c)
{
    return (c >= '0' && c <= '9');
}

int isalnum(int c)
{
    return isalpha(c) || isdigit(c);
}