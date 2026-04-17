CC = gcc

CFLAGS = -Wall -g

TARGETS = hostinfo mycat mydate pipe_ls printdir

all: $(TARGETS)

hostinfo: hostinfo.c
	$(CC) $(CFLAGS) -o hostinfo hostinfo.c

mycat: mycat.c
	$(CC) $(CFLAGS) -o mycat mycat.c

printdir: printdir.c
	$(CC) $(CFLAGS) -o printdir printdir.c

mydate: mydate.c
	$(CC) $(CFLAGS) -o mydate mydate.c

pipe_ls: pipe_ls.c
	$(CC) $(CFLAGS) -o pipe_ls pipe_ls.c

clean:
	rm -f $(TARGETS)