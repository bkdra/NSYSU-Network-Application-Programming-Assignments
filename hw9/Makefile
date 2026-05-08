CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
LDFLAGS = -lpthread -lrt

TARGET = prod_cons
SRC = prod_cons.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET) 4 5

.PHONY: all clean run
