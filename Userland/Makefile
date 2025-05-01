include Makefile.inc

USER_DATA=0001-userDataModule.bin

all: userCodeModule userDataModule

userCodeModule:
	cd UserCodeModule; make

userDataModule:
	printf "This is user data." >> $(USER_DATA) && dd if=/dev/zero bs=1 count=1 >> $(USER_DATA)

clean:
	cd UserCodeModule; make clean
	rm -rf *.bin *.elf


.PHONY: userCodeModule all clean
