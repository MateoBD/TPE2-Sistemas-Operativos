#include <syscall-dispatcher.h>
#include <stdarg.h>
#include <time.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <pcSpeakerDriver.h>

extern uint64_t * getRegisters();

enum enum_syscalls {
    SYS_WRITE = 0,
    SYS_READ,
    SYS_PIPE,
    SYS_CLOSE,
    SYS_SET_CURSOR,
    SYS_SET_COLOR,
    SYS_MMAP,
    SYS_MUNMAP,
    SYS_BRK,
    SYS_MPROTECT,
    SYS_FORK,
    SYS_EXECVE,
    SYS_EXIT,
    SYS_WAIT,
    SYS_GETPID,
    SYS_KILL,
    SYS_GETPRIORITY,
    SYS_SETPRIORITY,
    SYS_SCHED_YIELD,
    SYS_SLEEP,
    SYS_PLAY_SOUND,
    SYS_STOP_SOUND,
    SYS_SEM_OPEN,
    SYS_SEM_CLOSE,
    SYS_SEM_UNLINK,
    SYS_SEM_WAIT,
    SYS_SEM_POST,
    SYS_SEM_GETVALUE,
    SYS_SHM_OPEN,
    SYS_SHM_UNLINK,
    SYS_SHM_MAP,
    SYS_SHM_UNMAP,
};

#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
#define COLOR_RED 0xFF0000

uint32_t color = COLOR_WHITE;
uint32_t backgroundColor = COLOR_BLACK;
int cursorX=0, cursorY=0;

// Se setea el cursor [EN PIXELES]
uint64_t sys_setCursor(int x, int y) {
    cursorX = x;
    cursorY = y;
    return 1;
}

void setFontColor(uint32_t hexColor) {
    color = hexColor;
}

void changeBackgroundColor(uint32_t hexColor) {
    backgroundColor = hexColor;
}

void sys_write(FDS fd, const char *buf, uint64_t count) {
    ncNprint(buf, count);
    // if(fd == STDOUT || fd == STDERR) {
    //     int i = 0;
    //     while (i < count) {
    //         while (i < count && (cursorX) < DIM_X && buf[i] != '\n') {    
    //             drawchar(buf[i], cursorX, cursorY, (fd==STDOUT)?color:COLOR_RED, backgroundColor);
    //             cursorX++;
    //             i++;
    //         }
    //         if (buf[i] == '\n' || i < count) {
    //             cursorX = 0;
    //             cursorY++;
    //             i += (buf[i] == '\n');  // si tengo un salto de linea, salteo
    //         }
    //     }
    // }
    // return;
}

uint32_t readChars(char * buf, uint64_t count) {
    int i = 0, c;
    int end = 0;
    while (i < count && !end) {
        c = nextKey();
        buf[i++] = (char) c;
        end = (c == -2);
    }
    return i - end;             // si end es 1, resto un caracter (que seria el -2)
}

uint64_t sys_read(FDS fd, char *buf, uint64_t count) {
    if (fd == STDIN) {
        return readChars(buf, count);
    }
    return 0;
}

void sys_sleep(int seconds){
    sleep(seconds);
}

typedef uint64_t (*sys_call_t)(uint64_t rdi, uint64_t rsi, uint64_t r10, uint64_t r8);

sys_call_t sys_call_arr[] = 
{
    [SYS_WRITE] = (sys_call_t) sys_write,
    [SYS_READ] = (sys_call_t) sys_read,
    // [SYS_PIPE] = (sys_call_t) sys_pipe,
    // [SYS_CLOSE] = (sys_call_t) sys_close,
    // [SYS_SET_CURSOR] = (sys_call_t) sys_set_cursor,
    // [SYS_SET_COLOR] = (sys_call_t) sys_set_color,
    // [SYS_MMAP] = (sys_call_t) sys_mmap,
    // [SYS_MUNMAP] = (sys_call_t) sys_munmap,
    // [SYS_BRK] = (sys_call_t) sys_brk,
    // [SYS_MPROTECT] = (sys_call_t) sys_mprotect,
    // [SYS_FORK] = (sys_call_t) sys_fork,
    // [SYS_EXECVE] = (sys_call_t) sys_execve,
    // [SYS_EXIT] = (sys_call_t) sys_exit,
    // [SYS_WAIT] = (sys_call_t) sys_wait,
    // [SYS_GETPID] = (sys_call_t) sys_getpid,
    // [SYS_KILL] = (sys_call_t) sys_kill,
    // [SYS_GETPRIORITY] = (sys_call_t) sys_getpriority,
    // [SYS_SETPRIORITY] = (sys_call_t) sys_setpriority,
    // [SYS_SCHED_YIELD] = (sys_call_t) sys_sched_yield,
    // [SYS_SLEEP] = (sys_call_t) sys_sleep,
    // [SYS_PLAY_SOUND] = (sys_call_t) sys_play_sound,
    // [SYS_STOP_SOUND] = (sys_call_t) sys_stop_sound,
    // [SYS_SEM_OPEN] = (sys_call_t) sys_sem_open,
    // [SYS_SEM_CLOSE] = (sys_call_t) sys_sem_close,
    // [SYS_SEM_UNLINK] = (sys_call_t) sys_sem_unlink,
    // [SYS_SEM_WAIT] = (sys_call_t) sys_sem_wait,
    // [SYS_SEM_POST] = (sys_call_t) sys_sem_post,
    // [SYS_SEM_GETVALUE] = (sys_call_t) sys_sem_getvalue,
    // [SYS_SHM_OPEN] = (sys_call_t) sys_shm_open,
    // [SYS_SHM_UNLINK] = (sys_call_t) sys_shm_unlink,
    // [SYS_SHM_MAP] = (sys_call_t) sys_shm_map,
    // [SYS_SHM_UNMAP] = (sys_call_t) sys_shm_unmap,
};

uint64_t syscall_dispatcher(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r8) {
    uint64_t ret;

    ret = sys_call_arr[rax](rdi, rsi, rdx, r8);

    return ret;
}

// int itoa(uint64_t value, char * buffer, int base, int n) {
//     char *p = buffer;
// 	char *p1, *p2;
// 	uint32_t digits = 0;
// 	do {
// 		uint32_t remainder = value % base;
// 		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
// 		digits++;
//     } while (value /= base);
//     n -= digits;
//     while (n-- > 0) {
//         *p++ = '0';
//     }
//     *p = 0x00;
// 	p1 = buffer;
// 	p2 = p - 1;
// 	while (p1 < p2) {
// 		char tmp = *p1;
// 		*p1 = *p2;
// 		*p2 = tmp;
// 		p1++;
// 		p2--;
// 	}
// 	return digits;
// }

// char* strNCpy(const char *src, char *dest, int n) {
//     int i;
//     for (i = 0; i < n && src[i] != '\0'; i++) {
//         dest[i] = src[i];
//     }
//     for (; i < n; i++) {
//         dest[i] = '\0';
//     }
//     return dest;
// }

// int strLen(char * str) {
//     int i = 0;
//     while (str[i++] != '\0') {
//         ;
//     }
//     return i;
// }

// void showRegisters() {
//     uint64_t * reg = getRegisters();
//     char  strs[][4] = {"rax:", "rbx:", "rcx:", "rdx:", "rdi:", "rsi:", "rsp:", "rbp:", "r8: ", "r9: ", "r10:", "r11:", "r12:", "r13:", "r14:", "r15:", "rip:", "cs: ", "rfl:"};
//     char * buf = "\tRRRR 0xHHHHHHHHHHHHHHHH\n";
//     for (int i = 0; i < 19; i++) {
//         strNCpy(strs[i], buf+1, 4);
//         itoa(reg[i], buf+8, 16, 16);
//         buf[24] = '\n';
//         sys_write(STDERR, buf, 25);
//     }
// }
