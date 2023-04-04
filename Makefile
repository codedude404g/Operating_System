CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

all: sortuniqwc

sortuniqwc: sortuniqwc.c
	$(CC) $(CFLAGS) -o sortuniqwc sortuniqwc.c

clean:
	rm -f sortuniqwc
