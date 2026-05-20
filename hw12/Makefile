CC = gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lzmq

TARGETS = broker producer program worker monitor

.PHONY: all clean

all: $(TARGETS)

broker: broker.c msg_system.h
	$(CC) $(CFLAGS) broker.c -o $@ $(LDLIBS)

producer: producer.c msg_system.h
	$(CC) $(CFLAGS) producer.c -o $@ $(LDLIBS)

program: program.c msg_system.h
	$(CC) $(CFLAGS) program.c -o $@ $(LDLIBS)

worker: worker.c msg_system.h
	$(CC) $(CFLAGS) worker.c -o $@ $(LDLIBS)

monitor: monitor.c msg_system.h
	$(CC) $(CFLAGS) monitor.c -o $@ $(LDLIBS)

clean:
	rm -f $(TARGETS)