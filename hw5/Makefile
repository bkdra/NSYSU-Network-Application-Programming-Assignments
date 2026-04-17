CC = gcc
CFLAGS = -Wall -g

TARGETS = listPid pstree find_opener

all: $(TARGETS)

listPid: listPid.c
	$(CC) $(CFLAGS) -o listPid listPid.c

pstree: pstree.c
	$(CC) $(CFLAGS) -o pstree pstree.c

find_opener: find_opener.c
	$(CC) $(CFLAGS) -o find_opener find_opener.c

clean:
	rm -f $(TARGETS)