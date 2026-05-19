/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2026.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 44-7 */

/* fifo_seqnum_server.c

   An example of a server using a FIFO to handle client requests.
   The "service" provided is the allocation of unique sequential
   numbers. Each client submits a request consisting of its PID, and
   the length of the sequence it is to be allocated by the server.
   The PID is used by both the server and the client to construct
   the name of the FIFO used by the client for receiving responses.

   The server reads each client request, and uses the client's FIFO
   to send back the starting value of the sequence allocated to that
   client. The server then increments its counter of used numbers
   by the length specified in the client request.

   See fifo_seqnum.h for the format of request and response messages.

   The client is in fifo_seqnum_client.c.
*/
#include <signal.h>
#include "svmsg_seqnum.h"
#include <sys/msg.h>

int
main()
{
    int serverID, clientID;
    int seqNum = 0;                     /* This is our "service" */

    /* Create well-known FIFO, and open it for reading */

    serverID = msgget(SERVER_KEY, IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP);
    if (serverID == -1)
        errExit("msgget");

    printf("Server started. Waiting for connections...\n");
    
    for (;;) {                          /* Read requests and send responses */
        fflush(stdout);

        struct requestMsg clientMsg;
        int msgLen = msgrcv(serverID, &clientMsg, sizeof(struct requestMsg) - sizeof(long), 1, 0);
        if(msgLen == -1)
        {
            errMsg("msgrcv");
            continue;
        }

        /* Open client FIFO (previously created by client) */
        printf("Received Request -> PID:%d, SeqLen:%d\n", clientMsg.pid, clientMsg.seqLen);
        /* Send response and close FIFO */

        struct responseMsg respMsg;
        respMsg.mtype = 1;
        respMsg.seqNum = seqNum;

        clientID = clientMsg.clientId;
        if (msgsnd(clientID, &respMsg, sizeof(struct responseMsg) - sizeof(long), 0) == -1)
            errMsg("msgsnd");

        seqNum += clientMsg.seqLen;           /* Update our sequence number */
    }
}
