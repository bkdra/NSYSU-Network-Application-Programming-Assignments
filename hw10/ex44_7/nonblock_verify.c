#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/myfifo"

int main(int argc, char *argv[])
{
    // check argument
    // argv[1] is server mode, argv[2] is client mode
    // mode can be "block" or "nonblock"
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s [server mode] [client mode]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[1], "block") != 0 && strcmp(argv[1], "nonblock") != 0)
    {
        fprintf(stderr, "Invalid server mode. Use 'block' or 'nonblock'.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[2], "block") != 0 && strcmp(argv[2], "nonblock") != 0)
    {
        fprintf(stderr, "Invalid client mode. Use 'block' or 'nonblock'.\n");
        exit(EXIT_FAILURE);
    }
        
    umask(0);                           /* So we get the permissions we want */
    if (mkfifo(FIFO_PATH, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    int serverMode = strcmp(argv[1], "block") == 0 ? O_RDONLY : O_RDONLY | O_NONBLOCK;
    int clientMode = strcmp(argv[2], "block") == 0 ? O_WRONLY : O_WRONLY | O_NONBLOCK;

    int mode;
    if(strcmp(argv[1], "block") == 0 && strcmp(argv[2], "block") == 0)
    {
        mode = 0;
        printf("Both server and client are blocking on open.\n");
    }
        
    else if(strcmp(argv[1], "block") == 0 && strcmp(argv[2], "nonblock") == 0)
    {
        mode = 1;
        printf("Server is blocking on open, client is non-blocking.\n");
    }
    else if(strcmp(argv[1], "nonblock") == 0 && strcmp(argv[2], "block") == 0)
    {
        mode = 2;
        printf("Server is non-blocking on open, client is blocking.\n");
    }
    else
    {
        mode = 3;
        printf("Both server and client are non-blocking on open.\n");
    }
    
    int pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // child process: client
    {
        int fd;

        switch(mode)
        {
            case 0: // server: block, client: block
                // test client blocking on open
                printf("Client is blocking on open...\n");
                fd = open(FIFO_PATH, clientMode);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                printf("Client opened FIFO successfully.\n");
                close(fd);

                usleep(500000); // Sleep for 500 milliseconds to ensure no process is opening the fifo
                
                // test server blocking on open
                sleep(3); // sleep for a while to ensure server is blocking on open
                
                fd = open(FIFO_PATH, clientMode);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                printf("Client opened FIFO successfully.\n");
                close(fd);
                break;

            case 1: // server: block, client: nonblock
                // Client tries FIRST (before server opens) - should get ENXIO
                printf("Client attempting nonblocking open (server not ready yet)...\n");
                fd = open(FIFO_PATH, clientMode);
                if (fd == -1)
                {
                    if (errno == ENXIO)
                        printf("Client got ENXIO - no reader yet! (Expected behavior)\n");
                    else
                        perror("open");
                }
                else
                {
                    printf("Client opened FIFO successfully (unexpected).\n");
                    close(fd);
                }
                
                // Now wait for server to open and unblock, then try again
                sleep(3);
                printf("Client attempting nonblocking open again (server should be ready)...\n");
                fd = open(FIFO_PATH, clientMode);
                if (fd == -1)
                {
                    perror("open");
                }
                else
                {
                    printf("Client opened FIFO successfully (server ready).\n");
                    close(fd);
                }
                break;
            case 2: // server: nonblock, client: block
                sleep(1); // Give server time to try nonblocking first
                printf("Client attempting blocking open...\n");
                fd = open(FIFO_PATH, clientMode);
                if (fd == -1)
                    perror("open");
                else
                {
                    printf("Client opened FIFO successfully (blocking).\n");
                    close(fd);
                }
                break;
            case 3: // both: nonblock
                printf("Client attempting nonblocking open...\n");
                fd = open(FIFO_PATH, clientMode);
                if (fd == -1)
                {
                    if (errno == ENXIO)
                        printf("Client got ENXIO - no reader yet!\n");
                    else
                        perror("open");
                }
                else
                {
                    printf("Client opened FIFO successfully (nonblocking).\n");
                    close(fd);
                }
                break;
        }
    }
    else // parent process: server
    {
        int fd;

        switch(mode)
        {
            case 0:
                // test server blocking on open
                sleep(3); // sleep for a while to ensure client is blocking on open
                printf("Server joins the FIFO\n");
                fd = open(FIFO_PATH, serverMode);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                printf("Server opened FIFO successfully.\n");
                close(fd);

                usleep(500000); // Sleep for 500 milliseconds to ensure no process is opening the fifo

                printf("Server is blocking on open...\n");
                fd = open(FIFO_PATH, serverMode);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                printf("Server opened FIFO successfully.\n");
                close(fd);
                // test server blocking on open
                break;
            case 1: // server: block, client: nonblock
                sleep(1); // Give client time to try nonblocking first
                printf("Server blocking on open, waiting for client...\n");
                fd = open(FIFO_PATH, serverMode);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                printf("Server opened FIFO successfully (blocking).\n");
                close(fd);
                break;
            case 2: // server: nonblock, client: block
                printf("Server attempting nonblocking open...\n");
                fd = open(FIFO_PATH, serverMode);
                if (fd == -1)
                {
                    if (errno == ENXIO)
                        printf("Server got ENXIO - no writer yet!\n");
                    else
                        perror("open");
                }
                else
                {
                    printf("Server opened FIFO successfully (nonblocking).\n");
                    close(fd);
                }
                sleep(3); // Give client time to complete
                printf("server start to solve the blocking of client\n");
                fd = open(FIFO_PATH, serverMode);
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                printf("Server opened FIFO successfully (nonblocking).\n");
                close(fd);
                break;
            case 3: // both: nonblock
                sleep(1); // Give client time to try nonblocking first
                printf("Server attempting nonblocking open...\n");
                fd = open(FIFO_PATH, serverMode);
                if (fd == -1)
                {
                    if (errno == ENXIO)
                        printf("Server got ENXIO - no writer yet!\n");
                    else
                        perror("open");
                }
                else
                {
                    printf("Server opened FIFO successfully (nonblocking).\n");
                    close(fd);
                }
                sleep(1); // Give client time to complete
                break;
        }
    }
}