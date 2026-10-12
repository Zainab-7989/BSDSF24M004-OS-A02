CC = gcc
CFLAGS = -Wall -g

all: bin/ls

bin/ls: src/ls-v1.0.0.c
	mkdir -p bin obj
	$(CC) $(CFLAGS) src/ls-v1.0.0.c -o bin/ls

clean:
	rm -rf bin/* obj/*
