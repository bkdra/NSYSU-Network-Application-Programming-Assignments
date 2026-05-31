#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "zmqBroker.h"

Node *worker_queue = NULL;
Node *worker_queue_tail = NULL;
int main()
{
    void *context = zmq_ctx_new();
    if(context == NULL)
    {
        perror("zmq_ctx_new");
        exit(1);
    }
    void *socket = zmq_socket(context, ZMQ_ROUTER);
    if(socket == NULL)
    {
        perror("zmq_socket");
        exit(1);
    }
    if(zmq_bind(socket, "tcp://*:5555") == -1)
    {
        perror("zmq_bind");
        exit(1);
    }

    zmq_pollitem_t items[] = {
        {socket, 0, ZMQ_POLLIN, 0},
        {NULL, STDIO_FILENO, ZMQ_POLLIN, 0}
    };
    printf("Enter the number of truck, worker, insurance: ");
    while(1)
    {
        zmq_pool(items, 2, -1);
        if(items[0].revents & ZMQ_POLLIN)
        {
            char id_buffer[256], event_buffer[256];
            zmq_msg_t client_id_msg, event_msg;
            zmq_msg_init(&client_id_msg);
            zmq_msg_init(&event_msg);

            if(zmq_msg_recv(&client_id_msg, socket, 0) == -1)
            {
                perror("zmq_msg_recv");
                exit(1);
            }
            if(zmq_msg_recv(&event_msg, socket, 0) == -1)
            {
                perror("zmq_msg_recv");
                exit(1);
            }
            size_t client_id_msg_size = zmq_msg_size(&client_id_msg);
            size_t event_msg_size = zmq_msg_size(&event_msg);
            if(client_id_msg_size >= sizeof(id_buffer))
            {
                client_id_msg_size = sizeof(id_buffer) - 1;
            }
            if(event_msg_size >= sizeof(event_buffer))
            {
                event_msg_size = sizeof(event_buffer) - 1;
            }
            memcpy(id_buffer, zmq_msg_data(&client_id_msg), client_id_msg_size);
            id_buffer[client_id_msg_size] = '\0';
            memcpy(event_buffer, zmq_msg_data(&event_msg), event_msg_size);
            event_buffer[event_msg_size] = '\0';

            if(strcmp(event_buffer, "READY") == 0)
            {
                printf("Worker %s is ready\n", id_buffer);
                Enqueue(&worker_queue, &worker_queue_tail, atoi(id_buffer));
            }
            else if(strcmp(event_buffer, "RESULT") == 0)
            {
                printf("Worker %s is done\n", id_buffer);
                Enqueue(&worker_queue, &worker_queue_tail, atoi(id_buffer));
            }
        }
        if(items[1].revents & ZMQ_POLLIN)
        {
            int truck, people, insurance;
            scanf("%d %d %d", &truck, &people, &insurance);
            int worker_id = Dequeue(&worker_queue, &worker_queue_tail);
            if(worker_id != -1)
            {
                printf("Not have free worker: Task is not permitted\n");
            }
            else
            {
                printf("Assign task to worker %d\n", worker_id);
                zmq_msg_t client_id_msg, event_msg;
                zmq_msg_init_size(&client_id_msg, sizeof(Task));
                zmq_msg_init_size(&event_msg, sizeof(Task));
                Task task = {truck, people, insurance};
                memcpy(zmq_msg_data(&client_id_msg), &worker_id, sizeof(worker_id));
                memcpy(zmq_msg_data(&event_msg), &task, sizeof(Task));
                if(zmq_msg_send(&client_id_msg, socket, ZMQ_SNDMORE) == -1)
                {
                    perror("zmq_msg_send");
                    exit(1);
                }
                if(zmq_msg_send(&event_msg, socket, 0) == -1)
                {
                    perror("zmq_msg_send");
                    exit(1);
                }
                zmq_msg_close(&client_id_msg);
                zmq_msg_close(&event_msg);
            }   
        }
    }
}

void Enqueue(Node **queue, Node **queue_tail, int worker_id)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->worker_id = worker_id;
    new_node->next = NULL;

    if(*queue_tail == NULL)
    {
        *queue = new_node;
        *queue_tail = new_node;
    }
    else
    {
        (*queue_tail)->next = new_node;
        *queue_tail = new_node;
    }
}

int Dequeue(Node **queue, Node **queue_tail)
{
    if(*queue == NULL)
    {
        return -1; // Queue is empty
    }
    int worker_id = (*queue)->worker_id;
    Node *temp = *queue;
    *queue = (*queue)->next;
    if(*queue == NULL)
    {
        *queue_tail = NULL; // Queue is now empty
    }
    free(temp);
    return worker_id;
}