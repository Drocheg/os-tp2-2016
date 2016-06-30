all: build


build: dinosaurio.img

dinosaurio.img:

	gcc imageWriter.c -o imageWriter.o
	./imageWriter.o

clean:
	rm -rf *.o *.bin dinosaurio.img

.PHONY: all clean build
