#ifndef LATENCY_SHM_H
#define LATENCY_SHM_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

typedef struct
{
    int task_count;
    long long send_ns[];
} LatencyShm;

static inline long long latency_now_ns(void)
{
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
    {
        perror("clock_gettime");
        exit(1);
    }
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline size_t latency_shm_size(int task_count)
{
    return sizeof(LatencyShm) + (size_t)task_count * sizeof(long long);
}

static inline LatencyShm *latency_shm_create(const char *name, int task_count, int *fd_out)
{
    int fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0600);
    if(fd == -1)
    {
        return NULL;
    }

    if(ftruncate(fd, (off_t)latency_shm_size(task_count)) == -1)
    {
        perror("ftruncate");
        close(fd);
        shm_unlink(name);
        return NULL;
    }

    LatencyShm *shm = mmap(NULL, latency_shm_size(task_count), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        shm_unlink(name);
        return NULL;
    }

    shm->task_count = task_count;
    for(int i = 0; i < task_count; i++)
    {
        shm->send_ns[i] = -1;
    }

    if(fd_out != NULL)
    {
        *fd_out = fd;
    }
    else
    {
        close(fd);
    }

    return shm;
}

static inline LatencyShm *latency_shm_open(const char *name, int task_count, int *fd_out)
{
    int fd = shm_open(name, O_RDWR, 0600);
    if(fd == -1)
    {
        perror("shm_open");
        return NULL;
    }

    LatencyShm *shm = mmap(NULL, latency_shm_size(task_count), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return NULL;
    }

    if(fd_out != NULL)
    {
        *fd_out = fd;
    }
    else
    {
        close(fd);
    }

    return shm;
}

static inline void latency_shm_close(LatencyShm *shm, int fd)
{
    if(shm != NULL)
    {
        munmap(shm, latency_shm_size(shm->task_count));
    }
    if(fd >= 0)
    {
        close(fd);
    }
}

#endif