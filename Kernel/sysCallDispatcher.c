#include <sysCallDispatcher.h>
#include <stdarg.h>
#include <time.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <pcSpeakerDriver.h>

extern uint64_t * getRegisters();

#define SYSNUM_READ 0
#define SYSNUM_WRITE 1
#define SYSNUM_REGISTERS 2
#define SYSNUM_GET_TIME 4
#define SYSNUM_SET_CURSOR 5
#define SYSNUM_SET_FONT_COLOR 7
#define SYSNUM_SET_ZOOM 8
#define SYSNUM_DRAW_RECTANGLE 9
#define SYSNUM_SET_BACKGROUND_FONT_COLOR 10
#define SYSNUM_DRAW_SPRAY 11
#define SYSNUM_PLAY_SOUND 12
#define SYSNUM_STOP_SOUND 13
#define SYSNUM_GET_TICKS 14
#define SYSNUM_SLEEP 35

#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
#define COLOR_RED 0xFF0000

uint32_t color = COLOR_WHITE;
uint32_t backgroundColor = COLOR_BLACK;
int cursorX=0, cursorY=0;
uint8_t zoom = 1;

// Se setea el cursor [EN PIXELES]
size_t sys_setCursor(int x, int y) {
    cursorX = x;
    cursorY = y;
    return 1;
}

void setFontColor(uint32_t hexColor) {
    color = hexColor;
}

void sys_setZoom(int new_zoom) {
    zoom = new_zoom;
}
void changeBackgroundColor(uint32_t hexColor) {
    backgroundColor = hexColor;
}

void sys_write(FDS fd, const char *buf, size_t count) {
    if(fd == STDOUT || fd == STDERR) {
        int i = 0;
        while (i < count) {
            while (i < count && (cursorX+CHAR_WIDTH*zoom) < DIM_X && buf[i] != '\n') {    
                drawchar(buf[i], cursorX, cursorY, (fd==STDOUT)?color:COLOR_RED, backgroundColor, zoom);
                cursorX += CHAR_WIDTH*zoom;
                i++;
            }
            if (buf[i] == '\n' || i < count) {
                cursorX = 0;
                cursorY += CHAR_HEIGHT*zoom;
            }
            i += (buf[i] == '\n');  // si tengo un salto de linea, salteo
        }
    }
    return;
}

uint32_t readChars(char * buf, size_t count) {
    int i = 0, c;
    int end = 0;
    while (i < count && !end) {
        c = nextKey();
        buf[i++] = (char) c;
        end = (c == -2);
    }
    return i - end;             // si end es 1, resto un caracter (que seria el -2)
}

size_t sys_read(FDS fd, char *buf, size_t count) {
    if (fd == STDIN) {
        return readChars(buf, count);
    }
    return 0;
}

void sys_sleep(int seconds){
    sleep(seconds);
}

uint64_t sysCallDispatcher(uint64_t rax, ...) {
    va_list args;
    va_start(args, rax);
    uint64_t ret;
    if (rax == SYSNUM_READ) {
       uint64_t fd = va_arg(args, uint64_t);
       uint64_t buf = va_arg(args, uint64_t);
        uint64_t count = va_arg(args, uint64_t);
        ret = sys_read(fd, buf, count);
    } else if (rax == SYSNUM_WRITE) {
        FDS fd = va_arg(args, FDS);
        const char * buf = va_arg(args, const char*);
        uint64_t count = va_arg(args, uint64_t);
        sys_write(fd, buf, count);
        ret = 0;
    } else if (rax == SYSNUM_REGISTERS) {
        ret = getRegisters();
    } else if (rax == SYSNUM_GET_TIME) {
        ret = (uint64_t)getTime();
    } else if (rax == SYSNUM_SET_CURSOR) {
        int x = (int)va_arg(args, uint64_t);
        int y = (int)va_arg(args, uint64_t);
        sys_setCursor(x, y);
    } else if (rax == SYSNUM_SET_FONT_COLOR) {
        uint32_t hexColor = va_arg(args, uint32_t);
        setFontColor(hexColor);
    } else if (rax == SYSNUM_SET_ZOOM) {
        uint64_t new_zoom = va_arg(args, uint64_t);
        sys_setZoom(new_zoom);
    } else if (rax == SYSNUM_DRAW_RECTANGLE) {
        Point* p1 = va_arg(args, Point*);
        Point* p2 = va_arg(args, Point*);
        uint32_t c = va_arg(args, uint32_t);
        drawRectangle(p1,p2,c);
        ret = 0;
    } else if (rax == SYSNUM_SET_BACKGROUND_FONT_COLOR) {
        uint32_t hexColor = va_arg(args, uint32_t);
        changeBackgroundColor(hexColor);
    } else if (rax == SYSNUM_DRAW_SPRAY) {
        uint64_t spray = va_arg(args, uint64_t);
        uint64_t size_x = va_arg(args, uint64_t);
        uint64_t size_y = va_arg(args, uint64_t);
        drawSpray(size_x, size_y, spray, cursorX, cursorY);
    } else if (rax == SYSNUM_PLAY_SOUND) {
        uint64_t frecuency = va_arg(args, uint64_t);
        playSound(frecuency);
    } else if (rax == SYSNUM_STOP_SOUND) {
        stopSound();
    } else if (rax == SYSNUM_GET_TICKS) {
        ret=getTicks();
    } else if (rax == SYSNUM_SLEEP) {
        int seconds = va_arg(args, int);
        sys_sleep(seconds);    
    }
    va_end(args);
    return ret;
}

int itoa(uint64_t value, char * buffer, int base, int n) {
    char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;
	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
    } while (value /= base);
    n -= digits;
    while (n-- > 0) {
        *p++ = '0';
    }
    *p = 0x00;
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
	return digits;
}

char* strNCpy(const char *src, char *dest, int n) {
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

int strLen(char * str) {
    int i = 0;
    while (str[i++] != '\0') {
        ;
    }
    return i;
}

void showRegisters() {
    sys_setZoom(2);
    uint64_t * reg = getRegisters();
    char  strs[][4] = {"rax:", "rbx:", "rcx:", "rdx:", "rdi:", "rsi:", "rsp:", "rbp:", "r8: ", "r9: ", "r10:", "r11:", "r12:", "r13:", "r14:", "r15:", "rip:", "cs: ", "rfl:"};
    char * buf = "\tRRRR 0xHHHHHHHHHHHHHHHH\n";
    for (int i = 0; i < 19; i++) {
        strNCpy(strs[i], buf+1, 4);
        itoa(reg[i], buf+8, 16, 16);
        buf[24] = '\n';
        sys_write(STDERR, buf, 25);
    }
}
