#include Makefile.inc
include ../Tools.inc

MODULE=userland.bin
ASMSOURCES= asm/interrupts.asm
SOURCES= c/USERLAND.c c/piano.c c/songplayer.c c/usrlib.c c/game.c c/mq.c ../Common/c/scanCodes.c ../Common/c/stdlib.c
ASMOBJECTS= asm/interrupts.o
COBJECTS=c/USERLAND.o c/piano.o c/songplayer.o c/usrlib.o c/game.o c/mq.o ../Common/c/scanCodes.o ../Common/c/stdlib.o

all: $(USER) songmaker img

%.o: %.c
	$(GCC) -c $(GCCFLAGS) -I./include -I../Common/include/ $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(USER): $(COBJECTS) $(ASMOBJECTS)
	$(LD) -T userland.ld $(COBJECTS) $(ASMOBJECTS) -o $(MODULE)

songmaker:
	cd Songmaker; make

img:
	cd img; make

clean:
	rm -rf c/*.o asm/*.o *.o *.bin
	cd Songmaker; make clean

.PHONY: all clean songmaker img