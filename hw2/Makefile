# 指定編譯器
CC = gcc

# 編譯參數：-Wall 顯示所有警告, -g 加入除錯資訊
CFLAGS = -Wall -g

# 目標執行檔名稱
TARGETS = tee cp

# 預設動作：編譯所有目標
all: $(TARGETS)

# 編譯 tee
tee: tee.c
	$(CC) $(CFLAGS) -o tee tee.c

# 編譯 cp
cp: cp.c
	$(CC) $(CFLAGS) -o cp cp.c

# 清除編譯產出的檔案
clean:
	rm -f $(TARGETS)