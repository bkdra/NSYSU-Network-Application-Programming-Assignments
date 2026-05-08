CC = gcc
CFLAGS = -Wall -Wextra -std=gnu89 -g
LDFLAGS = -pthread

TARGET = main
SRCS = main_test_stack.c stack_cond.c stack_mutex_only.c utils.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c stack.h utils.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
