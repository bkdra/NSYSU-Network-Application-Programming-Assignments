#include "dict.h"
#include<zmq.h>
#include<string.h>

int main(int argc, char **argv) {
	// Dictrec dr, *tryit = &dr;
    Dictrec tryit;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <resource>\n",argv[0]);
		exit(1);
	}

	/* Fill in code. */
    void *context = zmq_ctx_new();
    if(context == NULL)
	{
		perror("zmq_ctx_new");
		exit(1);
	}	
    void *responder = zmq_socket(context, ZMQ_REP);
    if(responder == NULL )
	{
		perror("zmq_socket");
		exit(1);
	}
    char addr[32];
    snprintf(addr, sizeof(addr), "tcp://localhost:%d", BACKEND);
    if(zmq_connect(responder, addr) == -1)
    {
        perror("zmq_connect");
        exit(1);
    }


	for (;;) {
        /* Fill in code. */
        printf("Waiting for request...\n");
        if(zmq_recv(responder, &tryit, sizeof(Dictrec), 0) == -1)
        {
            perror("zmq_recv");
            exit(1);
        }
        printf("Received request for %s\n", tryit.word);
        switch (lookup(&tryit, argv[1])) {
            case FOUND:
                /* Fill in code. */
                if(zmq_send(responder, &tryit, sizeof(Dictrec), 0) == -1)
                {
                    perror("zmq_send");
                    exit(1);
                }
                break;

            case NOTFOUND:
                /* Fill in code. */
                strcpy(tryit.text, "XXXX");
                if(zmq_send(responder, &tryit, sizeof(Dictrec), 0) == -1)
                {
                    perror("zmq_send");
                    exit(1);
                }
                break;

            case UNAVAIL:
                DIE(argv[1]);
        }
    }
} /* end main */
