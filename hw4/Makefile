CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = shell
SRCS = shell.c builtin.c parse.c run_command.c is_background.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c shell.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
