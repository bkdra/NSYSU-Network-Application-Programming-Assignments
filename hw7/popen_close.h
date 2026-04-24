#define FD_SIZE 1024
#define CHILD_MAX 100

FILE* mypopen(const char* command, const char* mode);
int mypclose(FILE* stream);