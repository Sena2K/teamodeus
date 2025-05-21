CC = gcc
CFLAGS = -Wall -Wextra -O2

all: bfc bfe

generator: generator.c
	$(CC) $(CFLAGS) -o bfc bfc.c

interpreter: interpreter.c
	$(CC) $(CFLAGS) -o bfe bfe.c

clean:
	rm -f bfc bfe

.PHONY: all clean
