CC := tigcc
CFLAGS := -O2 -Wall -Iinclude

ifeq ($(OS),Windows_NT)
	RM := del
	/ := \\
else
	RM := rm -f
	/ := /
endif

ALL := cpu input screen emu main
SOURCES := $(patsubst %,src$(/)%.c,$(ALL))
HEADERS := $(patsubst %,include$(/)%.h,$(ALL))
OBJECTS := $(patsubst %,obj$(/)%.o,$(ALL))

.PHONY: all
all: ti8 any2ti

.PHONY: tipack
tipack: tipack/tipack.c
tipack/tipack.c:
	make -C tipack all

.PHONY: ti8
ti8: $(OBJECTS)
	$(CC) -o ti8 $^ $(CFLAGS)
obj$(/)%.o: src$(/)%.c $(SOURCES)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	$(RM) obj$(/)*.o ti8.9xz ti8.v2z
