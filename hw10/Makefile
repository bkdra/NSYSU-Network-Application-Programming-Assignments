CC = gcc
CFLAGS = -Wall -Wextra -g
CPPFLAGS = -I./ex44_6 -I./ex46_2

EX44_DIR = ex44_6
EX46_DIR = ex46_2
EX44_7_DIR = ex44_7

# Source and header files
EX44_HEADERS = $(EX44_DIR)/fifo_seqnum.h $(EX44_DIR)/tlpi_hdr.h $(EX44_DIR)/error_functions.h $(EX44_DIR)/get_num.h
EX44_SERVER_SRC = $(EX44_DIR)/server.c
EX44_CLIENT_SRC = $(EX44_DIR)/client.c

EX46_HEADERS = $(EX46_DIR)/svmsg_seqnum.h $(EX46_DIR)/tlpi_hdr.h $(EX46_DIR)/error_functions.h $(EX46_DIR)/get_num.h
EX46_SERVER_SRC = $(EX46_DIR)/server.c
EX46_CLIENT_SRC = $(EX46_DIR)/client.c

# ex44_7 standalone FIFO blocking/nonblocking verification program
EX44_7_SRC = $(EX44_7_DIR)/nonblock_verify.c

# Executables
EX44_SERVER_BIN = $(EX44_DIR)/ex44_6_srv
EX44_CLIENT_BIN = $(EX44_DIR)/ex44_6_cli

EX46_SERVER_BIN = $(EX46_DIR)/ex46_2_srv
EX46_CLIENT_BIN = $(EX46_DIR)/ex46_2_cli

EX44_7_BIN = $(EX44_7_DIR)/nonblock_verify

# Utility object files
EX44_UTIL_OBJS = $(EX44_DIR)/error_functions.o $(EX44_DIR)/get_num.o
EX46_UTIL_OBJS = $(EX46_DIR)/error_functions.o $(EX46_DIR)/get_num.o

# Targets
.PHONY: all clean

all: $(EX44_SERVER_BIN) $(EX44_CLIENT_BIN) $(EX46_SERVER_BIN) $(EX46_CLIENT_BIN) $(EX44_7_BIN)

$(EX44_SERVER_BIN): $(EX44_SERVER_SRC) $(EX44_UTIL_OBJS) $(EX44_HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(EX44_SERVER_SRC) $(EX44_UTIL_OBJS)

$(EX44_CLIENT_BIN): $(EX44_CLIENT_SRC) $(EX44_UTIL_OBJS) $(EX44_HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(EX44_CLIENT_SRC) $(EX44_UTIL_OBJS)

$(EX46_SERVER_BIN): $(EX46_SERVER_SRC) $(EX46_UTIL_OBJS) $(EX46_HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(EX46_SERVER_SRC) $(EX46_UTIL_OBJS)

$(EX46_CLIENT_BIN): $(EX46_CLIENT_SRC) $(EX46_UTIL_OBJS) $(EX46_HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(EX46_CLIENT_SRC) $(EX46_UTIL_OBJS)

$(EX44_7_BIN): $(EX44_7_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(EX44_7_SRC)

$(EX44_DIR)/error_functions.o: $(EX44_DIR)/error_functions.c $(EX44_DIR)/error_functions.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $(EX44_DIR)/error_functions.c

$(EX44_DIR)/get_num.o: $(EX44_DIR)/get_num.c $(EX44_DIR)/get_num.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $(EX44_DIR)/get_num.c

$(EX46_DIR)/error_functions.o: $(EX46_DIR)/error_functions.c $(EX46_DIR)/error_functions.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $(EX46_DIR)/error_functions.c

$(EX46_DIR)/get_num.o: $(EX46_DIR)/get_num.c $(EX46_DIR)/get_num.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $(EX46_DIR)/get_num.c

clean:
	rm -f $(EX44_SERVER_BIN) $(EX44_CLIENT_BIN) $(EX44_UTIL_OBJS) \
	      $(EX46_SERVER_BIN) $(EX46_CLIENT_BIN) $(EX46_UTIL_OBJS) \
	      $(EX44_7_BIN)
