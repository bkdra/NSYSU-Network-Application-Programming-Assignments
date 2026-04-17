CC = gcc
CFLAGS = -Wextra -std=c11 -g
TARGETS = convert file_lookup

CONVERT_SRCS = convert.c
CONVERT_OBJS = $(CONVERT_SRCS:.c=.o)

LOOKUP_SRCS = main.c lookup1.c
LOOKUP_OBJS = $(LOOKUP_SRCS:.c=.o)

.PHONY: all clean

all: $(TARGETS)

convert: $(CONVERT_OBJS)
	$(CC) $(CFLAGS) -o $@ $(CONVERT_OBJS)

file_lookup: $(LOOKUP_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LOOKUP_OBJS)

%.o: %.c dict.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CONVERT_OBJS) $(LOOKUP_OBJS) $(TARGETS)
