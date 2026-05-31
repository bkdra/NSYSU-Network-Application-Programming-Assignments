#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<zmq.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>
#include "latency_shm.h"

static pid_t broker_pid = -1;
static pid_t producer_pid = -1;
static pid_t *worker_pids = NULL;
static int worker_count = 0;
static volatile sig_atomic_t shutdown_requested = 0;
static char latency_shm_name[64];
static int latency_shm_fd = -1;
static LatencyShm *latency_shm = NULL;

static void terminate_children(void)
{
    if(broker_pid > 0)
    {
        kill(broker_pid, SIGTERM);
    }
    if(producer_pid > 0)
    {
        kill(producer_pid, SIGTERM);
    }
    for(int i = 0; i < worker_count; i++)
    {
        if(worker_pids[i] > 0)
        {
            kill(worker_pids[i], SIGTERM);
        }
    }
}

static void handle_sigint(int signo)
{
    (void)signo;
    shutdown_requested = 1;
    terminate_children();
}

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        fprintf(stderr, "Usage: %s <Task Count> <Task Interval> <Worker Count> <Queue Size>\n", argv[0]);
        exit(1);
    }
    char* task_count = argv[1];
    char* task_interval = argv[2];
    char* worker_count_str = argv[3];
    char* queue_size = argv[4];
    char worker_id_str[16];
    worker_count = atoi(worker_count_str);

    snprintf(latency_shm_name, sizeof(latency_shm_name), "/hw12_latency_%ld", (long)getpid());
    latency_shm = latency_shm_create(latency_shm_name, atoi(task_count), &latency_shm_fd);
    if(latency_shm == NULL)
    {
        fprintf(stderr, "failed to create shared memory for latency tracking\n");
        exit(1);
    }

    worker_pids = calloc(worker_count, sizeof(pid_t));
    if(worker_pids == NULL)
    {
        perror("calloc");
        exit(1);
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    
    broker_pid = fork();
    switch(broker_pid)
    {
        case -1:
            perror("fork");
            exit(1);
        case 0:
            execl("./broker", "./broker", latency_shm_name, task_count, task_interval, worker_count_str, queue_size, (char *)NULL);
            perror("execl");
            exit(1);
        default:
            break;
    }

    for(int i = 0; i < worker_count; i++)
    {
        worker_pids[i] = fork();
        switch(worker_pids[i])
        {
            case -1:
                perror("fork");
                exit(1);
            case 0:
                snprintf(worker_id_str, sizeof(worker_id_str), "%d", i + 1);
                execl("./worker", "./worker", worker_id_str, (char *)NULL);
                perror("execl");
                exit(1);
            default:
                break;
        }
    }

    producer_pid = fork();
    switch(producer_pid)
    {
        case -1:
            perror("fork");
            exit(1);
        case 0:
                    execl("./producer", "./producer", latency_shm_name, task_count, task_interval, (char *)NULL);
            perror("execl");
            exit(1);
        default:
            break;
    }

    

    while(!shutdown_requested)
    {
        int status;
        pid_t exited_pid = waitpid(-1, &status, 0);
        if(exited_pid == -1)
        {
            if(errno == EINTR)
            {
                continue;
            }
            if(errno == ECHILD)
            {
                break;
            }
            perror("waitpid");
            break;
        }

        if(exited_pid == broker_pid)
        {
            break;
        }
    }

    terminate_children();
    while(waitpid(-1, NULL, 0) > 0)
    {
    }

    latency_shm_close(latency_shm, latency_shm_fd);
    shm_unlink(latency_shm_name);
    free(worker_pids);
    return 0;
}