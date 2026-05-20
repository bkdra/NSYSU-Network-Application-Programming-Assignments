#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<zmq.h>
#include "msg_system.h"
#include<time.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <Queue Size> <Task Count>\n", argv[0]);
        exit(1);
    }
    int queue_size = atoi(argv[1]);
    int task_count = atoi(argv[2]);
    void *context = zmq_ctx_new();
 
    Node *worker_queue = NULL;
    Node *worker_queue_tail = NULL;
    Node *pending_task_queue = NULL;
    Node *pending_task_queue_tail = NULL;
    int worker_count = 0;
    int pending_task_count = 0;
    int dropped_task_count = 0;
    int processed_task_count = 0;

    if(context == NULL)
    {
        perror("zmq_ctx_new");
        exit(1);
    }
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    if(frontend == NULL)
    {
        perror("zmq_socket");
        exit(1);
    }
    void *backend = zmq_socket(context, ZMQ_ROUTER);
    if(backend == NULL)
    {
        perror("zmq_socket");
        exit(1);
    }
    void *monitor = zmq_socket(context, ZMQ_PUB);
    if(monitor == NULL)
    {
        perror("zmq_socket");
        exit(1);
    }
    if(zmq_bind(frontend, "tcp://*:5555") == -1)
    {
        perror("zmq_bind");
        exit(1);
    }
    if(zmq_bind(backend, "tcp://*:5556") == -1)
    {
        perror("zmq_bind");
        exit(1);
    }
    if(zmq_bind(monitor, "tcp://*:5557") == -1)
    {
        perror("zmq_bind");
        exit(1);
    }

    zmq_pollitem_t items[] = {
        {frontend, 0, ZMQ_POLLIN, 0},
        {backend, 0, ZMQ_POLLIN, 0}
    };

    time_t last_monitor_time = time(NULL);

    while(processed_task_count < task_count)
    {
        // printf("run\n");
        fflush(stdout);
        time_t current_time = time(NULL);
        if(difftime(current_time, last_monitor_time) >= 1.0)
        {

            printf("-------------------broker state: -------------------\n");
            printf("Queue size: %d\n", queue_size - pending_task_count);
            printf("Idle workers: %d\n", worker_count);
            printf("Processed: %d\n", processed_task_count);
            printf("Dropped: %d\n", dropped_task_count);
            printf("----------------------------------------------------\n");
            fflush(stdout);
            // send monitoring data to monitor
            char monitor_message[100];
                snprintf(monitor_message, 100, "QUEUE SIZE: %d, WORKERS: %d, PROCESSED: %d, DROPPED: %d", queue_size - pending_task_count, worker_count, processed_task_count, dropped_task_count);
                zmq_msg_t monitor_msg;
            zmq_msg_init_size(&monitor_msg, strlen(monitor_message) + 1);
            memcpy(zmq_msg_data(&monitor_msg), monitor_message, strlen(monitor_message) + 1);
            if(zmq_msg_send(&monitor_msg, monitor, 0) == -1)
            {
                perror("zmq_msg_send");
                exit(1);
            }
            zmq_msg_close(&monitor_msg);
            last_monitor_time = current_time;
        }
        
        zmq_poll(items, 2, -1);

        if(items[0].revents & ZMQ_POLLIN) // receive producer's task
        {
            // receive task from client

            char task[80];
            zmq_msg_t client_id_msg, task_msg;
            zmq_msg_init(&client_id_msg);
            zmq_msg_init(&task_msg);

            if(zmq_msg_recv(&client_id_msg, frontend, 0) == -1)
            {
                perror("zmq_msg_recv");
                exit(1);
            }
            if(zmq_msg_recv(&task_msg, frontend, 0) == -1)
            {
                perror("zmq_msg_recv");
                exit(1);
            }
            size_t task_size = zmq_msg_size(&task_msg);
            if(task_size >= sizeof(task))
            {
                task_size = sizeof(task) - 1;
            }
            memcpy(task, zmq_msg_data(&task_msg), task_size);
            task[task_size] = '\0';
            printf("broker receive task: %d\n", get_task_id(task));
            if(worker_queue != NULL)
            {
                // assign task to worker
                int worker_id = Dequeue(&worker_queue, &worker_queue_tail);
                worker_count--;
                printf("Assign task %d to worker %d\n", get_task_id(task), worker_id);
                if(worker_id != -1)
                {
                    // send task to worker
                    zmq_msg_t worker_identity;
                    zmq_msg_t task_to_worker;
                    zmq_msg_init_size(&worker_identity, sizeof(worker_id));
                    memcpy(zmq_msg_data(&worker_identity), &worker_id, sizeof(worker_id));
                    zmq_msg_init_size(&task_to_worker, strlen(task) + 1);
                    memcpy(zmq_msg_data(&task_to_worker), task, strlen(task) + 1);
                    if(zmq_msg_send(&worker_identity, backend, ZMQ_SNDMORE) == -1)
                    {
                        perror("zmq_msg_send");
                        exit(1);
                    }
                    if(zmq_msg_send(&task_to_worker, backend, 0) == -1)
                    {
                        perror("zmq_msg_send");
                        exit(1);
                    }
                    zmq_msg_close(&worker_identity);
                }
            }
            else
            {
                if(pending_task_count < queue_size)
                {
                    // enqueue task to pending task queue
                    printf("pend task %d (current pending task count: %d)\n", get_task_id(task), pending_task_count);
                    Enqueue(&pending_task_queue, &pending_task_queue_tail, get_task_id(task)); // using task id as worker id for simplicity
                    pending_task_count++;
                }
                else
                {
                    printf("Task queue is full. Discarding task: %s\n", task);
                    dropped_task_count++;
                }
            }
                zmq_msg_close(&client_id_msg);
            zmq_msg_close(&task_msg);
        }
        else if(items[1].revents & ZMQ_POLLIN) // receive from workers
        {
            // receive message from worker
            int worker_id;
            zmq_msg_t worker_id_msg, empty_msg, ready_msg;
            zmq_msg_init(&worker_id_msg);
            zmq_msg_init(&empty_msg);
            zmq_msg_init(&ready_msg);

            if(zmq_msg_recv(&worker_id_msg, backend, 0) == -1)
            {
                perror("zmq_msg_recv");
                exit(1);
            }
            memset(&worker_id, 0, sizeof(worker_id));
            memcpy(&worker_id, zmq_msg_data(&worker_id_msg), zmq_msg_size(&worker_id_msg));
            printf("-------------------worker_id: %d\n", worker_id);

            if(zmq_msg_recv(&empty_msg, backend, 0) == -1)
            {
                perror("zmq_msg_recv");
                exit(1);
            }

            if(zmq_msg_recv(&ready_msg, backend, 0) == -1)
            {
                perror("zmq_msg_recv");
                exit(1);
            }
            if(zmq_msg_size(&ready_msg) >= 4 && memcmp(zmq_msg_data(&ready_msg), "DONE", 4) == 0) // worker is complete its work
            {
                printf("worker %d is complete its work\n", worker_id);
                // worker is ready
                processed_task_count++;
                if(pending_task_queue != NULL)
                {
                    // assign pending task to worker
                    int pending_task_id = Dequeue(&pending_task_queue, &pending_task_queue_tail);
                    pending_task_count--;
                    if(pending_task_id != -1)
                    {
                        // send task to worker
                        zmq_msg_t task_to_worker;
                        zmq_msg_t worker_identity;
                        zmq_msg_init_size(&worker_identity, zmq_msg_size(&worker_id_msg));
                        memcpy(zmq_msg_data(&worker_identity), zmq_msg_data(&worker_id_msg), zmq_msg_size(&worker_id_msg));
                        zmq_msg_init_size(&task_to_worker, 80);
                        char task_buffer[80];
                        snprintf(task_buffer, 80, "TASK %d : This is message %d", pending_task_id, pending_task_id);
                        memcpy(zmq_msg_data(&task_to_worker), task_buffer, 80);
                        if(zmq_msg_send(&worker_identity, backend, ZMQ_SNDMORE) == -1)
                        {
                            perror("zmq_msg_send");
                            exit(1);
                        }
                        if(zmq_msg_send(&task_to_worker, backend, 0) == -1)
                        {
                            perror("zmq_msg_send");
                            exit(1);
                        }
                        zmq_msg_close(&worker_identity);
                    }
                }
                else
                {
                    printf("worker %d is ready\n", worker_id);
                    Enqueue(&worker_queue, &worker_queue_tail, worker_id);
                    worker_count++;
                }
            }
            else // READY
            {
                printf("worker %d start\n", worker_id);
                Enqueue(&worker_queue, &worker_queue_tail, worker_id);
                worker_count++;
            }
        }
    }
    
    printf("Total tasks: %d\n", task_count);
    printf("Processed tasks: %d\n", processed_task_count);
    printf("Dropped tasks: %d\n", dropped_task_count);
    printf("Loss rate: %1.2f%%\n", (float)dropped_task_count / task_count * 100);
    printf("Average latency: \n");
    zmq_close(frontend);
    zmq_close(backend);
    zmq_close(monitor);
    zmq_ctx_destroy(context);
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

int get_task_id(char *message)
{
    int task_id;
    sscanf(message, "TASK %d", &task_id);
    return task_id;
}