# $@ - Target name
# $^ - Target dependencies

CC = gcc
CFLAGS = -Wall -Wpedantic -Wextra --std=c89

img2aud: src/audio.c src/images.c src/img2aud.c
	$(CC) $^ $(CFLAGS) -g3 -o bin/$@
