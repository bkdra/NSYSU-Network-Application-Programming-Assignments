struct Task
{
    int truck;
    int people;
    int insurance;
};
typedef struct Task Task;

struct Node
{
    int worker_id;
    struct Node *next;
};
typedef struct Node Node;

void Enqueue(Node **queue, Node **queue_tail, int worker_id);
int Dequeue(Node **queue, Node **queue_tail);