CC=gcc
CFLAGS=-Wall
OFLAGS=-O2 -march=native
DFLAGS=-g3

.PHONY: all clean

all: main

main: main.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(OFLAGS) $(DFLAGS) -c $< -o $@

clean:
	rm -Rf *~ *.o main
