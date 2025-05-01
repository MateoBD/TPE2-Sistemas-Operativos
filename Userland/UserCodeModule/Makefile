include ../Makefile.inc

MODULE=0000-userCodeModule.bin
ELF_MODULE=0000-userCodeModule.elf
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard assembly/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
STATICLIBS=

all: $(MODULE)
	rm -rf $(OBJECTS) $(OBJECTS_ASM)

$(MODULE): $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(GCC) $(GCCFLAGS) -T userCodeModule.ld _loader.c -o ../$(MODULE) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(GCC) $(GCCFLAGS) -T userCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c -o ../$(ELF_MODULE) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf asm/*.o *.o

.PHONY: all clean print
