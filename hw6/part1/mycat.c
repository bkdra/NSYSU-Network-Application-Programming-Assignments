#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return EXIT_FAILURE;
    }
    char* filename = argv[1];

    int fd = open(filename, O_RDONLY);
    int n;
    char buffer[BUF_SIZE];
    while((n = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (write(STDOUT_FILENO, buffer, n) != n) 
        {
            perror("write");
            close(fd);
            return EXIT_FAILURE;
        }
    }
    if(n==-1)
    {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }

    return 0;
}