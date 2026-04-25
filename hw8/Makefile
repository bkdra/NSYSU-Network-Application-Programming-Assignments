CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g

TARGETS = sig_speed_sigsuspend sig_speed_sigwaitinfo test_systemvsigs
COMMON_OBJS = get_num.o error_functions.o
SIGSUSPEND_OBJS = sig_speed_sigsuspend.o $(COMMON_OBJS)
SIGWAITINFO_OBJS = sig_speed_sigwaitinfo.o $(COMMON_OBJS)

.PHONY: all clean test

all: $(TARGETS)


sig_speed_sigsuspend: $(SIGSUSPEND_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SIGSUSPEND_OBJS)

sig_speed_sigwaitinfo: $(SIGWAITINFO_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SIGWAITINFO_OBJS)

test_systemvsigs: test_systemvsigs.c SystemVsigs.c SystemVsigs.h
	$(CC) $(CFLAGS) -o $@ test_systemvsigs.c SystemVsigs.c

sig_speed_sigwaitinfo.o: sig_speed_sigwaitinfo.c tlpi_hdr.h get_num.h error_functions.h

sig_speed_sigsuspend.o: sig_speed_sigsuspend.c tlpi_hdr.h get_num.h error_functions.h
get_num.o: get_num.c get_num.h
error_functions.o: error_functions.c error_functions.h tlpi_hdr.h ename.c.inc

clean:
	rm -f $(TARGETS) $(TEST_TARGETS) *.o
