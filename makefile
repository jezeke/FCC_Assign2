CC = gcc
CFLAGS = -Wall -lm -Ofast

all: rsa keygen

rsa : rsa.c rsa.h
	$(CC) $(CFLAGS) rsa.c -o rsa

keygen : keygen.c
	$(CC) $(CFLAGS) keygen.c -o keygen

clean :
	rm rsa keygen
