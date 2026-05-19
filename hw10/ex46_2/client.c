/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2026.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 44-8 */

/* fifo_seqnum_client.c

   A simple client that uses a well-known FIFO to request a (trivial)
   "sequence number service". This client creates its own FIFO (using a
   convention agreed upon by client and server) which is used to receive a reply
   from the server. The client then sends a request to the server consisting of
   its PID and the length of the sequence it wishes to be allocated. The client
   then reads the server's response and displays it on stdout.

   See fifo_seqnum.h for the format of request and response messages.

   The server is in fifo_seqnum_server.c.
*/
#include "svmsg_seqnum.h"
#include <time.h>
#include <sys/msg.h>


int
main(int argc, char *argv[])
{
    int serverID, clientID;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [seq-len]\n", argv[0]);

    /* Create our FIFO (before sending request, to avoid a race) */
    serverID = msgget(SERVER_KEY, S_IWUSR);
    if (serverID == -1)
        errExit("msgget %s", SERVER_KEY);

    clientID = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientID == -1)
        errExit("msgget");

    /* Construct request message, open server FIFO, and send message */
    struct requestMsg req;
    req.mtype = 1;
    req.clientId = clientID;
    req.pid = getpid();
    req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;

    if(msgsnd(serverID, &req, sizeof(struct requestMsg) - sizeof(long), 0) == -1)
        errExit("msgsnd");
    
    printf("Sending request (PID=%d, SeqLen=%d), Waiting for response...\n", req.pid, req.seqLen);
    
    int msgLen;
    struct responseMsg resp;
    msgLen = msgrcv(clientID, &resp, sizeof(struct responseMsg) - sizeof(long), 1, 0);
    if(msgLen == -1)
        errExit("msgrcv");

    printf("Success! Assigned Sequence Number: %d\n", resp.seqNum);
    msgctl(clientID, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
