CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = tail
SRC = tail.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
