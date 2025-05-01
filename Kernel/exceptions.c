#include <syscalls.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE 6

extern void _hlt();
extern void _sti();
extern void _cli();
// static void zero_division();
// static void invalid_opcode();
// static void printExceptionMsg(char * e);

void exception_dispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID);
		// zero_division();
    if (exception == INVALID_OPCODE);
        // invalid_opcode();
}

// void waitToReturn() {
//     char c;
//     do {
//         _hlt();
//         sys_read(STDIN, &c, 1);
//     } while (c != '\n');
// }

// static void zero_division() {
//     _sti();
//     printExceptionMsg("Zero Division Error");
// }

// static void invalid_opcode() {
//     _sti();
//     printExceptionMsg("Invalid Opcode");
// }
