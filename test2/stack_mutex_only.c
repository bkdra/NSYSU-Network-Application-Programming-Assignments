/*****************************************************
This module exemplifies a threadsafe stack using
ONLY a mutex (no condition variables).

Instead of using pthread_cond_wait() to efficiently
sleep, threads use busy-waiting: continuously
locking/unlocking the mutex and checking the
condition. This is INEFFICIENT compared to using
condition variables, but demonstrates how a mutex
alone can coordinate threads.
/****************************************************/

#include <pthread.h> /* Posix threads. */
#include <stdio.h>
#include "stack.h"/* Stack function definitions. */
#include "utils.h"/* for printWithTime(). */

#define STACK_SIZE 3/* Define a small stack size to
cause contention. */

/* Define the data structure shared between the
threads. */

static char buffer[STACK_SIZE];/* Stack's buffer */
static int index = 0; /* Stack's index. */

/* Mutex lock for exclusive data access. */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


/***************************************************/
void my_push(char oneChar) {
/***************************************************
Push a character onto the stack. Return in the
second argument the stack index corresponding to
where the character is pushed.
/***************************************************/
char string[25];

/* Busy-wait using only mutex: keep trying until
stack is not full. This is inefficient! */
while (1) {
    /* Get the lock before accessing the shared data. */
    pthread_mutex_lock(&mutex);

    /* Test the data while under mutex protection, to
    see if the stack is pushable. */
    if (index < STACK_SIZE) {
        /* Stack is pushable. Push the data. */
        buffer[index++] = oneChar;

        sprintf (string,"Pushed:\tchar %c\tindex %d\n",
        oneChar, index-1);
        printWithTime(string);

        /* Release the mutex. All done with shared data
        access. */
        pthread_mutex_unlock(&mutex);
        break;
    }

    /* Stack is full. Release mutex and try again. */
    printWithTime ("push busy-waiting...\n");
    pthread_mutex_unlock(&mutex);
}
}

/***************************************************/
char my_pop() {
/***********************************************
Pop a character from the stack. Return in the
second argument the stack index corresponding to
where the character is popped.
/***************************************************/
char toReturn;
char string[25];

/* Busy-wait using only mutex: keep trying until
stack is not empty. This is inefficient! */
while (1) {
    /* Get the lock before accessing the shared data. */
    pthread_mutex_lock(&mutex);

    /* Test the data while under mutex protection
    to see if the stack is poppable. */
    if (index > 0) {
        /* Stack is poppable. Pop the data. */
        toReturn = buffer[--index];

        sprintf (string, "Pop:\tchar %c\tindex %d\n",
        toReturn, index);
        printWithTime(string);

        /* Release the mutex. All done with shared data
        access. */
        pthread_mutex_unlock(&mutex);
        return toReturn;
    }

    /* Stack is empty. Release mutex and try again. */
    printWithTime ("pop busy-waiting...\n");
    pthread_mutex_unlock(&mutex);
}
}
