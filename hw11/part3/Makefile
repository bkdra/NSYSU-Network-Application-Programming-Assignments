CC = gcc
CFLAGS = -Wall -g
LDLIBS = -lzmq

CLIENT = client
WORKER = worker
BROKER = broker

CLIENT_SRCS = main.c lookup9.c
WORKER_SRCS = zmq_worker.c lookup5.c
BROKER_SRCS = broker.c

CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
WORKER_OBJS = $(WORKER_SRCS:.c=.o)
BROKER_OBJS = $(BROKER_SRCS:.c=.o)

all: $(CLIENT) $(WORKER) $(BROKER)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(WORKER): $(WORKER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(BROKER): $(BROKER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c dict.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CLIENT_OBJS) $(WORKER_OBJS) $(BROKER_OBJS) $(CLIENT) $(WORKER) $(BROKER)

.PHONY: all clean