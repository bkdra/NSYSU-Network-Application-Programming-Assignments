struct Node
{
    int worker_id;
    struct Node *next;
};
typedef struct Node Node;

void Enqueue(Node **queue, Node **queue_tail, int worker_id);
int Dequeue(Node **queue, Node **queue_tail);
int get_task_id(char *message);