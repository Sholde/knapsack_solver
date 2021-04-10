CC=gcc
CFLAGS=-Wall
OFLAGS=-O2 -march=native
DFLAGS=-g3

.PHONY: all clean

all: ks

ks: main.o knapsack.o display.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(OFLAGS) $(DFLAGS) -c $< -o $@

clean:
	rm -Rf *~ *.o ks
