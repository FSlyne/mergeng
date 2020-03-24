CC=gcc
CFLAGS=-Wall  -std=c99 -g
LDFLAGS=-lm
DEPS=

all: main

main: main.c $(DEPS)
	$(CC) $(CFLAGS) main.c vpon.c random.c $(LDFLAGS) -o main
	
clean:
	rm main