CC = gcc
CFLAGS = -Wall -Wextra -g

.PHONY: all clean

all: main toCapital

main: main.c popen_close.c popen_close.h
	$(CC) $(CFLAGS) -o main main.c popen_close.c

toCapital: toCapital.c
	$(CC) $(CFLAGS) -o toCapital toCapital.c

clean:
	rm -f main toCapital *.o
