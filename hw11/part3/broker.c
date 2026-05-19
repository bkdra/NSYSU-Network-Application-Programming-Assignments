#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<zmq.h>
#include "dict.h"


int main()
{
    void *context = zmq_ctx_new();
    if(context == NULL)
	{
		perror("zmq_ctx_new");
		exit(1);
	}	
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    if(frontend == NULL )
	{
		perror("zmq_socket");
		exit(1);
	}
    void *backend = zmq_socket(context, ZMQ_DEALER);
    if(backend == NULL )
	{
		perror("zmq_socket");
		exit(1);
	}

    char frontend_addr[20], backend_addr[20];
    snprintf(frontend_addr, 20, "tcp://*:%d", FRONTEND);
    snprintf(backend_addr, 20, "tcp://*:%d", BACKEND);

    if(zmq_bind(frontend, frontend_addr) == -1)
    {
        perror("zmq_bind");
        exit(1);
    }
    if(zmq_bind(backend, backend_addr) == -1)
    {
        perror("zmq_bind");
        exit(1);
    }
    if(zmq_proxy(frontend, backend, NULL) == -1)
    {
        perror("zmq_proxy");
        exit(1);
    }

    // never reach this
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
}