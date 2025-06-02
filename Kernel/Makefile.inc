GCC=x86_64-linux-gnu-gcc
LD=x86_64-linux-gnu-ld
AR=x86_64-linux-gnu-ar
ASM=nasm

GCCFLAGS=-Werror -m64 -fno-pie -no-pie -fno-exceptions -fno-asynchronous-unwind-tables -mno-mmx -mno-sse -mno-sse2 -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -mno-red-zone -Wall -ffreestanding -nostdlib -fno-common -std=c99 -g
ARFLAGS=rvs
ASMFLAGS=-felf64 -g
LDFLAGS=--warn-common -z max-page-size=0x1000 -g
