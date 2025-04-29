#include <sysCallDispatcher.h>
#include <videoDriver.h>
#include <pcSpeakerDriver.h>
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE 6

extern void _hlt();
extern void _sti();
extern void _cli();
static void zero_division();
static void invalid_opcode();
static void printExceptionMsg(char * e);

void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
    if (exception == INVALID_OPCODE)
        invalid_opcode();
}

void waitToReturn() {
    char c;
    do {
        _hlt();
        sys_read(STDIN, &c, 1);
    } while (c != '\n');
}

static void zero_division() {
    _sti();
    printExceptionMsg("Zero Division Error");
}

static void invalid_opcode() {
    _sti();
    printExceptionMsg("Invalid Opcode");
}

static void printExceptionMsg(char * e) {

    drawRectangle(&((Point){0,0}), &((Point){DIM_X, DIM_Y}), 0x000000);
    sys_setZoom(2);
    sys_setCursor(8, 8);
    sys_write(STDERR, "Exception: ", 11);
    sys_write(STDERR, e, strLen(e));
    sys_write(STDERR, "\n\n", 2);
    showRegisters();
    playSound(277);
    sys_sleep(2);
    stopSound();
    sys_sleep(2);
    playSound(277);
    sys_sleep(2);
    stopSound();
    sys_sleep(14);
    sys_write(STDERR, "\n\tPress enter to return to the shell\n", 36);
    waitToReturn();
    drawRectangle(&((Point){0,0}), &((Point){DIM_X, DIM_Y}), 0x000000);
}
