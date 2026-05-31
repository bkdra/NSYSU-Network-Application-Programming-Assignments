#include <zmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "zmqBroker.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <Worker Number>\n", argv[0]);
        exit(1);
    }
    int worker_num = atoi(argv[1]);

    for(int i = 0; i < worker_num; i++)
    {
        pid_t pid = fork();
        if(pid == -1)
        {
            perror("fork");
            exit(1);
        }
        else if(pid == 0)
        {
            void *context = zmq_ctx_new();
            if(context == NULL)
            {
                perror("zmq_ctx_new");
                exit(1);
            }
            void *dispatcher_socket = zmq_socket(context, ZMQ_DEALER);
            void *broker_socket = zmq_socket(context, ZMQ_REQ);
            if(dispatcher_socket == NULL || broker_socket == NULL)
            {
                perror("zmq_socket");
                exit(1);
            }
            int workerID = i;
            zmq_setsockopt(dispatcher_socket, ZMQ_IDENTITY, &workerID, sizeof(workerID));
            if(zmq_connect(dispatcher_socket, "tcp://localhost:5555") == -1)
            {
                perror("zmq_connect");
                exit(1);
            }
            zmq_setsockopt(broker_socket, ZMQ_IDENTITY, &workerID, sizeof(workerID));
            if(zmq_connect(broker_socket, "tcp://localhost:5556") == -1)
            {
                perror("zmq_connect");
                exit(1);
            }
            // 先送個READY給dispatcher
            zmq_msg_t ready_msg;
            zmq_msg_init_size(&ready_msg, 6);
            memcpy(zmq_msg_data(&ready_msg), "READY", 6);
            if(zmq_msg_send(&ready_msg, dispatcher_socket, 0) == -1)
            {
                perror("zmq_msg_send");
                exit(1);
            }

            while(1)
            {
                zmq_msg_t client_id_msg, event_msg;
                zmq_msg_init(&client_id_msg);
                zmq_msg_init(&event_msg);

                if(zmq_msg_recv(&client_id_msg, dispatcher_socket, 0) == -1)
                {
                    perror("zmq_msg_recv");
                    exit(1);
                }
                if(zmq_msg_recv(&event_msg, dispatcher_socket, 0) == -1)
                {
                    perror("zmq_msg_recv");
                    exit(1);
                }
                Task task;
                memcpy(&task, zmq_msg_data(&event_msg), sizeof(Task));
                printf("Worker %d received task: truck=%d, people=%d, insurance=%d\n", workerID, task.truck, task.people, task.insurance);
                zmq_msg_close(&client_id_msg);
                zmq_msg_close(&event_msg);

                // request the resource from broker
                zmq_msg_t workerID, request_msg;
                zmq_msg_init_size(&request_msg, sizeof(Task));
                zmq_msg_init_size(&workerID, sizeof(workerID));
                memcpy(zmq_msg_data(&request_msg), &task, sizeof(Task));
                memcpy(zmq_msg_data(&workerID), &workerID, sizeof(workerID));
                // request the resource
                if(zmq_msg_send(&workerID, broker_socket, ZMQ_SNDMORE) == -1)
                {
                    perror("zmq_msg_send");
                    exit(1);
                }
                if(zmq_msg_send(&request_msg, broker_socket, 0) == -1)
                {
                    perror("zmq_msg_send");
                    exit(1);
                }
                if(zmq_msg_recv(&request_msg, broker_socket, 0) == -1)
                {
                    perror("zmq_msg_recv");
                    exit(1);
                }
                int permission;
                memcpy(&permission, zmq_msg_data(&request_msg), sizeof(int));
                if(permission)
                {
                    sleep(1); // 模擬工作時間
                    printf("Worker %d completed the task\n", workerID);
                    // release the resource
                    if(zmq_msg_send(&workerID, broker_socket, ZMQ_SNDMORE) == -1)
                    {
                        perror("zmq_msg_send");
                        exit(1);
                    }
                    if(zmq_msg_send(&request_msg, broker_socket, 0) == -1)
                    {
                        perror("zmq_msg_send");
                        exit(1);
                    }
                    if(zmq_msg_recv(&request_msg, broker_socket, 0) == -1)
                    {
                        perror("zmq_msg_recv");
                        exit(1);
                    }
                }
                else
                {
                    printf("Broker cannot afford the resource request\n");
                }

                zmq_msg_close(&workerID);
                zmq_msg_close(&request_msg);

                // 工作完成後送DONE給dispatcher
                zmq_msg_t done_msg;
                zmq_msg_init_size(&done_msg, 5);
                memcpy(zmq_msg_data(&done_msg), "DONE", 5);
                if(zmq_msg_send(&done_msg, dispatcher_socket, 0) == -1)
                {
                    perror("zmq_msg_send");
                    exit(1);
                }
                zmq_msg_close(&done_msg);

            }
            break;
        }
    }
}