include Makefile.inc

KERNEL=kernel.bin
ELF_KERNEL=kernel.elf
MODULES= . drivers scheduler syscalls memory-manager interrupts semaphores pipes kernel-libs
SOURCES=$(foreach mod, $(MODULES), $(wildcard $(mod)/*.c))
INCLUDES=$(addprefix -I, $(addsuffix /include, $(MODULES)))
ifeq ($(BUDDY),1)
    MM_FILE := buddymm.c
    GCCFLAGS += -Dbuddy
    $(info [KERNEL] Usando buddy allocator: $(MM_FILE))
else
    MM_FILE := memory-manager.c
    $(info [KERNEL] Usando default allocator: $(MM_FILE))
endif

# Removemos ambos archivos del memory manager (si están) y agregamos solo el correcto
SOURCES := $(filter-out memory-manager/buddymm.c memory-manager/memory-manager.c, $(SOURCES))
SOURCES += memory-manager/$(MM_FILE)  # Añadir con la ruta completa

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
