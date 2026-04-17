#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include<sys/stat.h> 

#define BUFSIZE 1000000
int main(int argc, char *argv[])
{
    if (argc != 3) return 1;

    char buf[BUFSIZE];
    int fd_src = open(argv[1], O_RDONLY);
    int fd_dst = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644);

    int num = read(fd_src, buf, BUFSIZE);
    buf[num] = '\0';
    
    for(int i=0;i<num;i++)
    {
        if(buf[i] != '\0')
        {
            if(write(fd_dst, &buf[i], 1) < 0)
            {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if(lseek(fd_dst, 1, SEEK_CUR) < 0)
            {
                perror("lseek");
                exit(EXIT_FAILURE);
            }
        }
    }

    struct stat st;
    if(fstat(fd_src, &st)==0)
        ftruncate(fd_dst, st.st_size);

    close(fd_src);
    close(fd_dst);

    return 0;
}