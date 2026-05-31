#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main()
{
    int worker[] = {1, 2, 5, 10, 20, 50, 100};
    for(int i=0;i<7;i++)
    {
        pid_t pid = fork();
        switch(pid)
        {
            case -1:
                perror("fork");
                exit(1);
            case 0:
                char q[16];
                snprintf(q, sizeof(q), "%d", worker[i]);
                execl("./program", "./program", "5000", "50", q, "10", (char *)NULL);
                exit(0);
            default:
                wait(NULL);
                break;
        }
    }
}