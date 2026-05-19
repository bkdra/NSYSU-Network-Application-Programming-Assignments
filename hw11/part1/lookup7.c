/*
 * lookup7 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by ZeroMQ IPC REQ/REP sockets
 * The name of the socket is passed as resource.
 */

 // call by main.c (client). only use for send request to server and return answer back to main after receive server's reply.

#include <zmq.h>
#include <string.h>
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int first_time = 1;
	static void *context;
	static void *requester;

	int n;

	if (first_time) {
		first_time = 0;
		context = zmq_ctx_new();
		if(context == NULL)
		{
			perror("zmq_ctx_new");
			exit(1);
		}	
		requester = zmq_socket(context, ZMQ_REQ);
		if(requester == NULL )
		{
			perror("zmq_socket");
			exit(1);
		}
		char addr[32];
    	snprintf(addr, sizeof(addr), "ipc:///tmp/dict_socket");
		if(zmq_connect(requester, addr) == -1)
		{
			perror("zmq_connect");
			exit(1);
		}
		/* Fill in code. */
	}

	/* Fill in code. */
	if(zmq_send(requester, sought, sizeof(Dictrec), 0) == -1)
	{
		perror("zmq_send");
		exit(1);
	}
	printf("Sent request for %s\n", sought->word);
	n = zmq_recv(requester, sought, sizeof(Dictrec), 0);
	if (n == -1) 
	{
		perror("zmq_recv");
		exit(1);
	}
	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
