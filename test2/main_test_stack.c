#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "stack.h"

#define ITEMS_PER_THREAD 8

static void *cond_producer(void *arg) {
    int i;
    char base = (char)(long)arg;

    for (i = 0; i < ITEMS_PER_THREAD; i++) {
        push((char)(base + i));
        usleep(30000);
    }

    return NULL;
}

static void *cond_consumer(void *arg) {
    int i;
    (void)arg;

    for (i = 0; i < ITEMS_PER_THREAD; i++) {
        (void)pop();
        usleep(50000);
    }

    return NULL;
}

static void *mutex_producer(void *arg) {
    int i;
    char base = (char)(long)arg;

    for (i = 0; i < ITEMS_PER_THREAD; i++) {
        my_push((char)(base + i));
        usleep(30000);
    }

    return NULL;
}

static void *mutex_consumer(void *arg) {
    int i;
    (void)arg;

    for (i = 0; i < ITEMS_PER_THREAD; i++) {
        (void)my_pop();
        usleep(50000);
    }

    return NULL;
}

int main(void) {
    pthread_t t1;
    pthread_t t2;

    puts("=== Condition-variable stack: push/pop ===");
    pthread_create(&t1, NULL, cond_producer, (void *)(long)'A');
    pthread_create(&t2, NULL, cond_consumer, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    puts("=== Mutex-only stack: my_push/my_pop ===");
    pthread_create(&t1, NULL, mutex_producer, (void *)(long)'a');
    pthread_create(&t2, NULL, mutex_consumer, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    puts("=== Done ===");
    return 0;
}
