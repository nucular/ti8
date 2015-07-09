CC := tigcc
CFLAGS := -O2 -Wall -Iinclude

ifeq ($(OS),Windows_NT)
	RM := del
	/ := \\
else
	RM := rm -f
	/ := /
endif

ALL := input cpu screen emulator main
SOURCES := $(patsubst %,src$(/)%.c,$(ALL))
HEADERS := $(patsubst %,include$(/)%.h,$(ALL))
OBJECTS := $(patsubst %,obj$(/)%.o,$(ALL))

all: ti8
ti8: $(OBJECTS)
	$(CC) -o bin$(/)$@ $^ $(CFLAGS)

obj$(/)%.o: src$(/)%.c $(SOURCES)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	$(RM) obj$(/)*.o bin$(/)*.9xz bin$(/)*.v2z
