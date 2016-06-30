all: build

build: song1.bin

song1.bin:
	gcc songmaker.c -o songmaker.o
	./songmaker.o

clean:
	rm -rf *.o *.bin

.PHONY: all clean build
