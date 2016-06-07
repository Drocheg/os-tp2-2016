MP=mp.bin
SOURCES=$(wildcard *.c)

all: $(MP)

$(MP): $(SOURCES)
	gcc $(SOURCES) -o $(MP) -largp

clean:
	rm -rf $(MP)

.PHONY: all clean
