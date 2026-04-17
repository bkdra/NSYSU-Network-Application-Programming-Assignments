#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>

int main() {
    int p[2];
    int pid;

    if ( pipe(p) == -1 ) {
        perror("pipe");
        exit(1);
    }
    switch( pid = fork() ) {
        case -1:
            perror("fork");
            exit(1);
            break;
        case 0:
            close(p[0]);
            if (p[1] != 1) {
                dup2(p[1], 1);
                close(p[1]);
            }
            execlp("ls","ls", "-l",(char *)NULL);
            perror("execlp");
            exit(1);
            break;
        default:
            break;
    }
    close(p[1]);
    if (waitpid(pid, (int *)NULL, 0) == -1) {
        perror("waitpid");
        exit(1);
    }

    char buf[1024];
    int n;
    while((n = read(p[0], buf, sizeof(buf))) > 0)
    {
        if (write(STDOUT_FILENO, buf, n) != n) 
        {
            perror("write");
            close(p[0]);
            return EXIT_FAILURE;
        }
    }
    if(n==-1)
    {
        perror("read");
        close(p[0]);
        return EXIT_FAILURE;
    }

    close(p[0]);
    return 0;
}