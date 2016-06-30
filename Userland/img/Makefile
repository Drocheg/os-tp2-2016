all: build

build: image.img

image.img:
	gcc imageWriter.c -o imageWriter.o
	./imageWriter.o

clean:
	rm -rf *.o *.bin

.PHONY: all clean build
