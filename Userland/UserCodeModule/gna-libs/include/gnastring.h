#ifndef _GNASTRING_H_
#define _GNASTRING_H_

int strlen(const char *str);

void strcpy(char *dest, const char *src);

void strncpy(char *dest, const char *src, int n);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, int n);

char *strchr(const char *str, int c);

int strcasecmp(const char *s1, const char *s2);

int strncasecmp(const char *s1, const char *s2, int n);

int tolower(int c);

int toupper(int c);

int isalpha(int c);

int isdigit(int c);

int isalnum(int c);

int is_vowel(int c);

#endif