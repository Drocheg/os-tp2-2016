all: bootloader common userland kernel song image

bootloader:
	cd Bootloader; make all

common:
	cd Common; make all

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

song:
	cd Userland/Songmaker; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Common; make clean
	cd Userland; make clean
	cd Kernel; make clean
	cd Userland/Songmaker; make clean
	cd Image; make clean

.PHONY: all clean bootloader common userland kernel song image 