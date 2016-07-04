#include Makefile.inc
include ../Tools.inc


KERNEL= kernel.bin
SOURCES = c/idt.c c/KERNEL.c c/kernel-lib.c c/modules.c c/video-text.c c/video-graphics.c c/interruptHandlers.c c/keyboard.c c/moduleLoader.c c/speaker.c c/memory.c c/time.c c/interrupts.c c/process.c c/scheduler.c c/basicFile.c c/fileManager.c c/mq.c c/tty.c c/rawKeys.c c/lock.c
OBJECTS = c/idt.o c/KERNEL.o c/kernel-lib.o c/modules.o c/video-text.o c/video-graphics.o c/interruptHandlers.o c/keyboard.o c/moduleLoader.o c/speaker.o c/memory.o c/time.o c/interrupts.o c/process.o c/scheduler.o c/basicFile.o c/fileManager.o c/mq.o c/tty.o c/rawKeys.o c/lock.o
SOURCES_ASM = asm/interrupts.asm asm/libasm.asm asm/scheduler.asm asm/lock.asm
OBJECTS_ASM = asm/interrupts.o asm/libasm.o asm/scheduler.o asm/lock.o
SOURCES_COMMON = ../Common/c/scanCodes.c ../Common/c/stdlib.c ../Common/c/video-common.c
OBJECTS_COMMON = ../Common/c/scanCodes.o ../Common/c/stdlib.o ../Common/c/video-common.o

LOADERSRC = asm/loader.asm
LOADEROBJECT = asm/loader.o

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS_COMMON) $(OBJECTS) $(OBJECTS_ASM)
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