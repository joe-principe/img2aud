# $@ - Target name
# $^ - Target dependencies

CC = gcc
CFLAGS = -Wall -Wpedantic -Wextra

img2aud: img2aud.c
	$(CC) $^ $(CFLAGS) -g3 -o bin/$@
