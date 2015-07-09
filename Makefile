CC := tigcc
CFLAGS := -O2 -Wall -Iinclude

ALL := main
SOURCES := $(patsubst %,src/%.c,$(ALL))
HEADERS := $(patsubst %,include/%.h,$(ALL))
OBJECTS := $(patsubst %,obj/%.o,$(ALL))

ifeq ($(OS),Windows_NT)
	RM := del
	/ := \\
else
	RM := rm -f
	/ := /
endif

all: $(OBJECTS)
	$(CC) -o bin$(/)$@ $^ $(CFLAGS)

obj/%.o: src$(/)%.c $(SOURCES)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	$(RM) obj$(/)*.o bin$(/)*.9xz bin$(/)*.v2z
