#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "popen_close.h"

static pid_t child_pid[FD_SIZE];
static int child_pos[CHILD_MAX] = {-1};
static int child_count = 0;

FILE* mypopen(const char* command, const char* mode)
{

    int pd[2];
    if(pipe(pd) == -1)
    {
        printf("Pipe failed\n");
        return NULL;
    }
    
    pid_t pid = fork();
    if(pid == -1)
    {
        printf("Fork failed\n");
        return NULL;
    }
    else if(pid == 0) /* child process */
    {
        for(int i = 0; i < child_count; i++)
        {
            close(child_pid[child_pos[i]]);
        }
        if(strcmp(mode, "r") == 0)
        {
            close(pd[0]);
            child_pid[pd[1]] = pid;
            child_pos[child_count++] = pd[1];
            dup2(pd[1], STDOUT_FILENO);
            close(pd[1]);
        }
        else if(strcmp(mode, "w") == 0)
        {
            close(pd[1]);
            child_pid[pd[0]] = pid;
            child_pos[child_count++] = pd[0];
            dup2(pd[0], STDIN_FILENO);
            close(pd[0]);
        }
        else
        {
            printf("Invalid mode\n");
            exit(1);
        }
        execl("/bin/sh", "sh", "-c", command, (char*)NULL);
        printf("execl failed\n");
        exit(1);
    }
    else /* parent process */
    {
        if(strcmp(mode, "r") == 0)
        {
            close(pd[1]);
            return fdopen(pd[0], mode);
        }
        else if(strcmp(mode, "w") == 0)
        {
            close(pd[0]);
            return fdopen(pd[1], mode);
        }
        else
        {
            printf("Invalid mode\n");
            return NULL;
        }
    }
}

int mypclose(FILE* stream)
{
    int fd = fileno(stream);
    int status;

    fclose(stream);
    waitpid(child_pid[fd], &status, 0);

    return WEXITSTATUS(status);
}