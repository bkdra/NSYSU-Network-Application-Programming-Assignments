#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<zmq.h>
#include "latency_shm.h"

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        fprintf(stderr, "Usage: %s <Shared Memory Name> <Task Count> <Task Interval>\n", argv[0]);
        exit(1);
    }

    const char *latency_shm_name = argv[1];
    int task_count = atoi(argv[2]);
    int task_interval = atoi(argv[3]);
    int task_sequence = 0;
    int shm_fd = -1;
    LatencyShm *latency_shm = latency_shm_open(latency_shm_name, task_count, &shm_fd);
    if(latency_shm == NULL)
    {
        exit(1);
    }
    

    void *context = zmq_ctx_new();
    if(context == NULL)
    {
        perror("zmq_ctx_new");
        exit(1);
    }
    void *producer = zmq_socket(context, ZMQ_DEALER);
    if(producer == NULL)
    {
        perror("zmq_socket");
        exit(1);
    }
    if(zmq_connect(producer, "tcp://localhost:5555") == -1)
    {
        perror("zmq_connect");
        exit(1);
    }
    // printf("task_interval: %d ms\n", task_interval);
    while(task_sequence < task_count)
    {
        latency_shm->send_ns[task_sequence] = latency_now_ns();
        zmq_msg_t message;
        char buffer[80];
        snprintf(buffer, 80, "TASK %d : This is message %d", task_sequence, task_sequence);
        zmq_msg_init_size(&message, strlen(buffer) + 1);
        memcpy(zmq_msg_data(&message), buffer, strlen(buffer) + 1);
        // printf("send task %d\n", task_sequence);
        if(zmq_msg_send(&message, producer, 0) == -1)
        {
            perror("zmq_msg_send");
            exit(1);
        }
        zmq_msg_close(&message);
        task_sequence++;
        usleep(task_interval * 1000);
    }
    
    latency_shm_close(latency_shm, shm_fd);
    zmq_close(producer);
    zmq_ctx_destroy(context);
}
