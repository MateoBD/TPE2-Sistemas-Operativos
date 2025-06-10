/* _loader.c */
#include <stdint.h>
extern char bss;
extern char endOfBinary;

int main();

void * memset(void * destiny, int32_t c, uint64_t length);

int _start() 
{
	memset(&bss, 0, &endOfBinary - &bss);
	return main();
}
