#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "zmqBroker.h"

Node *pending_req_queue = NULL;
Node *pending_req_queue_tail = NULL;

int main()
{
    const int total_truck = 5;
    const int total_people = 12;
    const int total_insurance = 1000;
    int curr_truck = total_truck;
    int curr_people = total_people;
    int curr_insurance = total_insurance;

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
    if(zmq_bind(socket, "tcp://*:5556") == -1)
    {
        perror("zmq_bind");
        exit(1);
    }

    while(1)
    {
        zmq_msg_t id_msg, req_msg;
        zmq_msg_init(&id_msg);
        zmq_msg_init(&req_msg);
        if(zmq_msg_recv(&id_msg, socket, 0) == -1)
        {
            perror("zmq_msg_recv");
            exit(1);
        }
        if(zmq_msg_recv(&req_msg, socket, 0) == -1)
        {
            perror("zmq_msg_recv");
            exit(1);
        }
        int worker_id;
        Task task;
        memcpy(&worker_id, zmq_msg_data(&id_msg), sizeof(worker_id));
        memcpy(&task, zmq_msg_data(&req_msg), sizeof(Task));
        printf("Broker received request from worker %d: truck=%d, people=%d, insurance=%d\n", worker_id, task.truck, task.people, task.insurance);
        zmq_msg_close(&id_msg);
        zmq_msg_close(&req_msg);
        if(task.truck <= total_truck && task.people <= total_people && task.insurance <= total_insurance)
        {   
            if(task.truck <= curr_truck && task.people <= curr_people && task.insurance <= curr_insurance)
            {
                curr_truck -= task.truck;
                curr_people -= task.people;
                curr_insurance -= task.insurance;
                // grant the resource

                printf("Broker granted the resource to worker %d\n", worker_id);
                zmq_msg_t reply_msg;
                zmq_msg_init_size(&reply_msg, sizeof(int));
                int permission = 1;
                memcpy(zmq_msg_data(&reply_msg), &permission, sizeof(permission));
                if(zmq_msg_send(&reply_msg, socket, 0) == -1)
                {
                    perror("zmq_msg_send");
                    exit(1);
                }
               zmq_msg_close(&reply_msg);

                // wait for worker to release the resource
                zmq_msg_t release_id_msg, release_msg;
                zmq_msg_init(&release_id_msg);
                zmq_msg_init(&release_msg);
                if(zmq_msg_recv(&release_id_msg, socket, 0) == -1)
                {
                    perror("zmq_msg_recv");
                    exit(1);
                }
                if(zmq_msg_recv(&release_msg, socket, 0) == -1)
                {
                    perror("zmq_msg_recv");
                    exit(1);
                }
                int release_worker_id;
                Task release_task;
                memcpy(&release_worker_id, zmq_msg_data(&release_id_msg), sizeof(release_worker_id));
                memcpy(&release_task, zmq_msg_data(&release_msg), sizeof(Task));
                printf("Broker received release from worker %d: truck=%d, people=%d, insurance=%d\n", release_worker_id, release_task.truck, release_task.people, release_task.insurance);
                zmq_msg_close(&release_id_msg);
                zmq_msg_close(&release_msg);
                curr_truck += release_task.truck;
                curr_people += release_task.people;
                curr_insurance += release_task.insurance;   
            
                // after releasing the resource, check if there is pending request in the queue
                if(pending_req_queue != NULL)
                {
                    

                }
            }
        }
        else
        {
             // deny the resource
             printf("Broker cannot afford the resource request from worker %d\n", worker_id);
             zmq_msg_t reply_msg;
             zmq_msg_init_size(&reply_msg, sizeof(int));
             int permission = 0;
             memcpy(zmq_msg_data(&reply_msg), &permission, sizeof(permission));
             if(zmq_msg_send(&reply_msg, socket, 0) == -1)
             {
                 perror("zmq_msg_send");
                 exit(1);
             }
             zmq_msg_close(&reply_msg);
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