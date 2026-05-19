CC = gcc
CFLAGS = -Wall -g
LDLIBS = -lzmq

CLIENT = dict_lookup
SERVER = zmq_tcp_server

CLIENT_SRCS = main.c lookup8.c
SERVER_SRCS = zmq_tcp_server.c lookup5.c

CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c dict.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT) $(SERVER)

.PHONY: all clean
