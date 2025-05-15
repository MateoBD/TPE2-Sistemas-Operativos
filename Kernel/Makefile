include Makefile.inc

KERNEL=kernel.bin
ELF_KERNEL=kernel.elf
MODULES= . drivers scheduler syscalls
SOURCES=$(foreach mod, $(MODULES), $(wildcard $(mod)/*.c))
INCLUDES=$(addprefix -I, $(addsuffix /include, $(MODULES)))
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)
	rm -rf $(OBJECTS) $(OBJECTS_ASM) $(LOADEROBJECT)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(ELF_KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) $(INCLUDES) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	@echo "Eliminando todos los archivos objeto y binarios..."
	rm -rf asm/*.o $(foreach mod,$(MODULES),$(mod)/*.o) *.bin *.elf

.PHONY: all clean
