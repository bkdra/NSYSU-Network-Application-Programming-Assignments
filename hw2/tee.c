#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    char buf[BUFSIZE];
    ssize_t num;

    num = read(0, buf, BUFSIZE);
    if (num < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    buf[num] = '\0';

    int fd;
    int isAppend = 0, opt;
    while((opt = getopt(argc, argv, "a")) != -1) {
        switch(opt) {
            case 'a':
                isAppend = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-a] filename\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if(isAppend == 1)
    {
        fd = open(argv[2], O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    else
    {
        fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }

    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (write(fd, buf, num) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    if (write(1, buf, num) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(fd);

    return 0;
}
