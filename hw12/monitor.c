#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<zmq.h>
#include<unistd.h>

int main()
{
    void *context = zmq_ctx_new();
    if(context == NULL)
    {
        perror("zmq_ctx_new");
        exit(1);
    }
    void *socket = zmq_socket(context, ZMQ_SUB);
    if(socket == NULL)
    {
        perror("zmq_socket");
        exit(1);
    }
    if(zmq_connect(socket, "tcp://localhost:5557") == -1)
    {
        perror("zmq_connect");
        exit(1);
    }

    zmq_msg_t msg;
    zmq_msg_init(&msg);
    while(1)
    {
        if(zmq_msg_recv(&msg, socket, 0) == -1)
        {
            perror("zmq_msg_recv");
            exit(1);
        }
        int queuesize, workers, processed, dropped;
        sscanf(zmq_msg_data(&msg), "QUEUE SIZE: %d, WORKERS: %d, PROCESSED: %d, DROPPED: %d", &queuesize, &workers, &processed, &dropped);
        printf("[Monitor]\n");
        printf("Queue size: %d\n", queuesize);
        printf("Idle workers: %d\n", workers);
        printf("Processed: %d\n", processed);
        printf("Dropped: %d\n", dropped);
        zmq_msg_close(&msg);
    }
}