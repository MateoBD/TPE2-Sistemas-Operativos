// #include <libc.h>
// #include <libasm.h>

// typedef enum{
//     STDIN = 0,
//     STDOUT,
//     STDERR,
// } FDS;
// #define READ 0
// #define WRITE 1
// #define REGISTERS 2
// #define GET_TIME 4
// #define SET_CURSOR 5
// #define SET_FONT_COLOR 7
// #define SET_ZOOM 8
// #define DRAW_RECTANGLE 9
// #define SET_BACKGROUND_FONT_COLOR 10
// #define DRAW_SPRAY 11
// #define GET_TICKS 14
// #define SLEEP 35

// uint64_t * getRegisters() {
//     return sys_call(REGISTERS, 0, 0, 0, 0);
// }

// void showRegisters() {
//     uint64_t * reg = getRegisters();
//     if (reg == NULL) {
//         print("\tRegisters not saved yet\n");
//         print("\tPress 'esc' to save them\n");
//         return;
//     }
//     char  strs[][4] = {"rax:", "rbx:", "rcx:", "rdx:", "rdi:", "rsi:", "rsp:", "rbp:", "r8: ", "r9: ", "r10:", "r11:", "r12:", "r13:", "r14:", "r15:", "rip:", "cs: ", "rfl:"};
//     char * buf = "\tRRRR 0xHHHHHHHHHHHHHHHH\n";
//     for (int i = 0; i < 19; i++) {
//         strNCpy(strs[i], buf+1, 4);
//         itoa(reg[i], buf+8, 16, 16);
//         buf[24] = '\n';
//         print(buf);
//     }
// }

// void timeToStr(char * buf) { 
//     time * t = getTime();
//     strCpy("dd/mm/yy 00:00:00", buf);
//     char aux[3] = {0x00};
//     itoa(t->day, aux, 16, 2);
//     strNCpy(aux, buf, 2);
//     itoa(t->month, aux, 16, 2);
//     strNCpy(aux, buf+3, 2);
//     itoa(t->year, aux, 16, 2);
//     strNCpy(aux, buf+6, 2);
//     itoa(t->hour, aux, 16, 2);
//     strNCpy(aux, buf+9, 2);
//     itoa(t->min, aux, 16, 2);
//     strNCpy(aux, buf+12, 2);
//     itoa(t->sec, aux, 16, 2);
//     strNCpy(aux, buf+15, 2);
// }