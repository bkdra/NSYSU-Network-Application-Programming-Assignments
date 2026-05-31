#include<stdio.h>
#include<zmq.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
    zmq_pollitem_t items[] = {
        {NULL, STDIN_FILENO, ZMQ_POLLIN, 0}
    };
    while(1)
    {
        zmq_poll(items, 1, -1);
        if(items[0].revents & ZMQ_POLLIN)
        {
            int num;
            scanf("%d", &num);
            printf("num = %d\n", num);
        }
    }
}