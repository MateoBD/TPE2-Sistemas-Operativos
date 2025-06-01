buddy:
	$(MAKE) all USE_BUDDY=1

all:  bootloader kernel userland image

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; \
	if [ "$$USE_BUDDY" = "1" ]; then \
		echo "Building with buddy allocator"; \
		$(MAKE) all BUDDY=1; \
	else \
		echo "Building with default allocator"; \
		$(MAKE) all; \
	fi


userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean
