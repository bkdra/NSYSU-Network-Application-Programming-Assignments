#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <sys/wait.h>

#define DIE(x) perror(x),exit(1)

struct shared_data{
    pid_t pid;
    int random_num;
};

int sems;
struct shared_data *shared_memory = NULL;
int *shared_index = NULL;
int shmid_stack;
int shmid_index;
key_t key = 1234;

void push(pid_t pid, int random_num, struct shared_data *data, int max_size)
{
    struct sembuf sem_op_wait; 
    struct sembuf sem_op_post;

    sem_op_wait.sem_num = 0; // mutex semaphore index
    sem_op_wait.sem_op = -1; // Wait (decrement)
    sem_op_wait.sem_flg = 0;
    sem_op_post.sem_num = 0;
    sem_op_post.sem_op = 1; // Signal (increment)
    sem_op_post.sem_flg = 0;

    semop(sems, &sem_op_wait, 1); // Wait on mutex semaphore (lock the index and stack)
    while (*shared_index == max_size-1) {
        printf("Stack is full, producer is waiting...\n");
        semop(sems, &sem_op_post, 1); // Release mutex before waiting
        usleep(200000); // sleep for 0.2 seconds
        semop(sems, &sem_op_wait, 1); // Wait on mutex semaphore again
    }

    // push a data pair into the stack if it's not full
    if (*shared_index < max_size-1) {
        data[*shared_index].pid = pid;
        data[*shared_index].random_num = random_num;
        *shared_index = *shared_index + 1;
    }
    semop(sems, &sem_op_post, 1); // Signal mutex semaphore (unlock the index and stack)
}

struct shared_data pop(struct shared_data *data)
{
    struct sembuf sem_op_wait; 
    struct sembuf sem_op_post;

    sem_op_wait.sem_num = 0; // mutex semaphore index
    sem_op_wait.sem_op = -1; // Wait (decrement)
    sem_op_wait.sem_flg = 0;
    sem_op_post.sem_num = 0;
    sem_op_post.sem_op = 1; // Signal (increment)
    sem_op_post.sem_flg = 0;

    semop(sems, &sem_op_wait, 1); // Wait on mutex semaphore  (lock the index and stack)
    while (*shared_index == 0) {
        printf("Stack is empty, consumer is waiting...\n");
        semop(sems, &sem_op_post, 1); // Release mutex before waiting
        usleep(200000); // sleep for 0.2 seconds
        semop(sems, &sem_op_wait, 1); // Wait on mutex semaphore again
    }
    if (*shared_index > 0) {
        *shared_index = *shared_index - 1;
        semop(sems, &sem_op_post, 1); // Signal mutex semaphore  (unlock the index and stack)
        return data[*shared_index];
    }
    // [never arrive this] not thing can be poped, just return an empty struct and release the mutex
    semop(sems, &sem_op_post, 1); 
    struct shared_data empty = {-1, 0};
    return empty; // Return an empty struct if buffer is empty
}

int main(int argc, char **argv)
{
    // parse the command line arguments
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <num_producers> <num_items_each_producer>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int M = atoi(argv[1]); // number of producers
    int N = atoi(argv[2]); // number of item each producer produces

    // build semaphore set with 2 semaphores: mutex and fullCount
    sems = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
    if (sems == -1) {
        DIE("Failed to create semaphore set");
    }
    // Initialize mutex semaphore to 1 and fullCount semaphore to 0
    if (semctl(sems, 0, SETVAL, 1) == -1) {
        DIE("Failed to initialize mutex semaphore");
    }
    if (semctl(sems, 1, SETVAL, 0) == -1) {
        DIE("Failed to initialize fullCount semaphore");
    }
    
    
    // build shared memory segment 
    shmid_stack = shmget(key, sizeof(struct shared_data) * M, IPC_CREAT | 0666);
    if (shmid_stack == -1) {
        DIE("Failed to create shared memory segment");
    }
    shmid_index = shmget(key + 1, sizeof(int), IPC_CREAT | 0666);
    if (shmid_index == -1) {
        DIE("Failed to create shared memory segment for index");
    }

    shared_memory = shmat(shmid_stack, NULL, 0);
    shared_index = shmat(shmid_index, NULL, 0);
    if (shared_memory == (void *)-1 || shared_index == (void *)-1) {
        DIE("Failed to attach shared memory");
    }

    // start the producer and consumer processes
    for(int i=0 ; i<M ; i++) // fork M producer processes
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            DIE("Fork failed");
        }
        else if(pid == 0) // Producer process
        {
            printf("Producer P%d (PID %d) starts writing...\n", i+1, getpid());
            srand(getpid());
            for(int j=0 ; j<N ; j++)
            {
                int random_num = rand() % 10; // Generate a random number
                push(getpid(), random_num, (struct shared_data *)shared_memory, M);
                // sem_post(&fullCount); // Signal that an item is produced
                
                struct sembuf sem_op;
                sem_op.sem_num = 1; // fullCount semaphore index
                sem_op.sem_op = 1; // Increment fullCount
                sem_op.sem_flg = 0;
                if (semop(sems, &sem_op, 1) == -1) // signal that an item is produced
                    DIE("Failed to increment fullCount semaphore");

                printf("P%d writes (%d, %d), increments Sem 1.\n", i+1, getpid(), random_num);
                sleep((float)(rand() % 10 + 1) / 10.0); // Simulate time taken to produce an item
            }
            exit(0);
        }
    }

    // after forking M producer processes, the main process becomes the consumer
    printf("Consumer (PID %d) waits on Semaphore 1 (Full Count)\n", getpid());
    srand(getpid());
    int sum = 0;
    for(int i=0 ; i<M*N ; i++)
    {
        // sem_wait(&fullCount); // Wait for an item to be produced
        struct sembuf sem_op;
        sem_op.sem_num = 1; // fullCount semaphore index
        sem_op.sem_op = -1; // Decrement fullCount
        sem_op.sem_flg = 0;
        if (semop(sems, &sem_op, 1) == -1)
            DIE("Failed to decrement fullCount semaphore");
        struct shared_data item = pop((struct shared_data *)shared_memory);
        sum += item.random_num;
        printf("Consumer reads (%d, %d). Sum = %d\n", item.pid, item.random_num, sum);
        sleep((float)(rand() % 10 + 1) / 10.0); // Simulate time taken to consume an item
    }

    // Wait for all producer processes to finish
    for(int i=0;i<M;i++)
        wait(NULL);


    // clean the shared memory and semaphores
    shmdt(shared_memory); // Detach from shared memory
    shmctl(shmid_stack, IPC_RMID, NULL); // Clean up shared memory
    shmdt(shared_index); // Detach from shared memory for index
    shmctl(shmid_index, IPC_RMID, NULL); // Clean up shared memory for index

    semctl(sems, 0, IPC_RMID); // Clean up mutex semaphore
    semctl(sems, 1, IPC_RMID); // Clean up fullCount semaphore

    // print the result
    printf("\n");
    printf("Total Data Points Processed: %d\n", M*N);
    printf("Final Cumulative Sum = %d\n", sum);
}