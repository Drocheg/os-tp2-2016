#include Makefile.inc
include ../Tools.inc

COMMON = ../Common/c/scanCodes.c ../Common/c/stdlib.c
KERNEL= kernel.bin
SOURCES= c/idt.c c/KERNEL.c c/kernel-lib.c c/modules.c c/video.c c/interruptHandlers.c c/keyboard.c c/moduleLoader.c c/speaker.c
SOURCES_ASM= asm/interrupts.asm asm/libasm.asm
OBJECTS= c/idt.o c/KERNEL.o c/kernel-lib.o c/modules.o c/video.o c/interruptHandlers.o c/keyboard.o c/moduleLoader.o c/speaker.o
OBJECTS_ASM= asm/interrupts.o asm/libasm.o
OBJECTS_COMMON=../Common/c/scanCodes.o ../Common/c/stdlib.o
LOADERSRC= asm/loader.asm
LOADEROBJECT=asm/loader.o

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(COMMON) $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS_COMMON) $(OBJECTS) $(OBJECTS_ASM)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I../Common/include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o c/*.o *.o *.bin

.PHONY: all clean