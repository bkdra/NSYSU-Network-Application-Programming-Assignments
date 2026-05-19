#include <signal.h>
#include "fifo_seqnum.h"

int
main()
{
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0;                     /* This is our "service" */

    /* Create well-known FIFO, and open it for reading */

    umask(0);                           /* So we get the permissions we want */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1
            && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);
    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);

    /* Open an extra write descriptor, so that we never see EOF */

    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1)
        errExit("open %s", SERVER_FIFO);

    /* Let's find out about broken client pipe via failed write() */

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)    errExit("signal");

    printf("Server started. Waiting for connections...\n");
    
    for (;;) {                          /* Read requests and send responses */
        fflush(stdout);
        if (read(serverFd, &req, sizeof(struct request))
                != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;                   /* Either partial read or error */
        }

        /* Open client FIFO (previously created by client) */
        printf("Received Request -> PID:%d, SeqLen:%d\n", req.pid, req.seqLen);
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                (long) req.pid);
        clientFd = open(clientFifo, O_WRONLY | O_NONBLOCK);
        if (clientFd == -1) {           /* Open failed, give up on client */
            if(errno == ENXIO)
            {
                fprintf(stderr, "Client doesn't open the fifo: %s\n", clientFifo);
                printf("try again...");
                fflush(stdout);
                usleep(100000); // Sleep for 100 milliseconds before trying again, because client maybe would open the fifo but not yet.
                clientFd = open(clientFifo, O_WRONLY | O_NONBLOCK);
                if(clientFd == -1)
                {
                    if(errno == ENXIO)
                    {
                        fprintf(stderr, "Client still doesn't open the fifo: %s\n", clientFifo);
                    }
                    else
                    {
                        errMsg("open %s", clientFifo);
                    }
                    continue;
                }
                else
                {
                    printf(" Success open the FIFO\n");
                }
            }
            else
            {
                errMsg("open %s", clientFifo);
                continue;
            }
            
        }

        /* Send response and close FIFO */

        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response))
                != sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        if (close(clientFd) == -1)
            errMsg("close");

        seqNum += req.seqLen;           /* Update our sequence number */
    }
}
