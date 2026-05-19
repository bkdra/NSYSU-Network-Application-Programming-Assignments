#include "fifo_seqnum.h"
#include <time.h>

static char clientFifo[CLIENT_FIFO_NAME_LEN];

static void             /* Invoked on exit to delete client FIFO */
removeFifo(void)
{
    unlink(clientFifo);
}

int
main(int argc, char *argv[])
{
    int serverFd, clientFd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [seq-len]\n", argv[0]);

    /* Create our FIFO (before sending request, to avoid a race) */

    umask(0);                   /* So we get the permissions we want */
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
            (long) getpid());
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1
                && errno != EEXIST)
        errExit("mkfifo %s", clientFifo);

    if (atexit(removeFifo) != 0)
        errExit("atexit");

    printf("Connecting to server...\n");

    /* Construct request message, open server FIFO, and send message */
    req.pid = getpid();
    req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);

    
    printf("Sending request (PID=%d, SeqLen=%d...)\n", req.pid, req.seqLen);
    if (write(serverFd, &req, sizeof(struct request)) !=
            sizeof(struct request))
        fatal("Can't write to server");
    printf("Request sent!\n");
    

    /* Open our FIFO, read and display response */
    srand(time(NULL));
    int randNum = rand() % 10 + 1;
    // printf("randNum = %d\n", randNum);
    if((randNum > 3))
    {
        printf("client open the FIFO\n");
        clientFd = open(clientFifo, O_RDONLY);
    }
    else
    {
        printf("client doesn't open the FIFO\nTest server's handling of client that doesn't open the FIFO\n");
        fflush(stdout);
        usleep(500000);
        exit(EXIT_SUCCESS);
    }
    if (clientFd == -1)
        errExit("open %s", clientFifo);

    if (read(clientFd, &resp, sizeof(struct response))
            != sizeof(struct response))
        fatal("Can't read response from server");

    printf("Success! Assigned Sequence Number: %d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}
