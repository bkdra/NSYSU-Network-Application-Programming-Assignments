#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<zmq.h>
#include<unistd.h>
#include "msg_system.h"

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <Worker ID>\n", argv[0]);
        exit(1);
    }

    int worker_id = atoi(argv[1]);
    void *context = zmq_ctx_new();
    if(context == NULL)
    {
        perror("zmq_ctx_new");
        exit(1);
    }
    void *worker = zmq_socket(context, ZMQ_DEALER);
    if(worker == NULL)
    {
        perror("zmq_socket");
        exit(1);
    }
    zmq_setsockopt(worker, ZMQ_IDENTITY, &worker_id, sizeof(worker_id));
    if(zmq_connect(worker, "tcp://localhost:5556") == -1)
    {
        perror("zmq_connect");
        exit(1);
    }

    // send ready signal to broker
    zmq_msg_t empty, ready_message;
    zmq_msg_init(&empty);
    zmq_msg_init_size(&ready_message, 5);
    memcpy(zmq_msg_data(&ready_message), "READY", 5);

    if(zmq_msg_send(&empty, worker, ZMQ_SNDMORE) == -1)
    {
        perror("zmq_msg_send");
        exit(1);
    }
    if(zmq_msg_send(&ready_message, worker, 0) == -1)
    {
        perror("zmq_msg_send");
        exit(1);
    }
    zmq_msg_close(&ready_message);

    // start working
    while(1)
    {
        zmq_msg_t message, done_message;
        zmq_msg_init(&message);
        if(zmq_msg_recv(&message, worker, 0) == -1)
        {
            perror("zmq_msg_recv");
            exit(1);
        }
        char buffer[80];
        size_t message_size = zmq_msg_size(&message);
        if(message_size >= sizeof(buffer))
        {
            message_size = sizeof(buffer) - 1;
        }
        memcpy(buffer, zmq_msg_data(&message), message_size);
        buffer[message_size] = '\0';
        int task_id = get_task_id(buffer); // parse task id from message
        
        int randNum = rand() % 2000001 + 1000000; // random number between 500000 and 1000000
        usleep(randNum); // simulate work by sleeping for random time
        
        zmq_msg_init_size(&done_message, 5+sizeof(int));
        memcpy(zmq_msg_data(&done_message)+5, &task_id, sizeof(int));
        memcpy(zmq_msg_data(&done_message), "DONE ", 5);
        if(zmq_msg_send(&empty, worker, ZMQ_SNDMORE) == -1)
        {
            perror("zmq_msg_send");
            exit(1);
        }
        if(zmq_msg_send(&done_message, worker, 0) == -1)
        {
            perror("zmq_msg_send");
            exit(1);
        }
        zmq_msg_close(&message);
    }
    zmq_close(worker);
    zmq_ctx_destroy(context);
}

int get_task_id(char *message)
{
    int task_id;
    sscanf(message, "TASK %d", &task_id);
    return task_id;
}