CC := gcc
CFLAGS := -O3 -Wall

ifeq ($(OS),Windows_NT)
	RM := del
	EXE := .exe
	/ := \\
else
	RM := rm -f
	EXE :=
	/ := /
endif

all: tipack

.PHONY: tipack
tipack: tipack.c
tipack.c:
	$(CC) -o tipack$(EXE) main.c $(CFLAGS)

clean:
	$(RM) tipack$(EXE)
