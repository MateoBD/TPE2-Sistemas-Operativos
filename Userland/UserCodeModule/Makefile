include ../Makefile.inc

MODULE=0000-userCodeModule.bin
ELF_MODULE=0000-userCodeModule.elf
MODULES= . gna-libs assembly
SOURCES=$(foreach mod, $(filter-out assembly, $(MODULES)), $(wildcard $(mod)/[^_]*.c))
INCLUDES=$(addprefix -I, $(addsuffix /include, $(MODULES)) ./include)
SOURCES_ASM=$(wildcard assembly/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
ELF_MODULE=0000-userCodeModule.elf
STATICLIBS=

all: $(MODULE)
	rm -rf $(OBJECTS) $(OBJECTS_ASM)

$(MODULE): $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(GCC) $(GCCFLAGS) -T userCodeModule.ld _loader.c -o ../$(MODULE) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(GCC) $(GCCFLAGS) -T userCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c -o ../$(ELF_MODULE) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) $(INCLUDES) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	@echo "Eliminando todos los archivos objeto y binarios..."
	rm -rf $(foreach mod,$(MODULES),$(mod)/*.o) *.bin *.elf

.PHONY: all clean print
