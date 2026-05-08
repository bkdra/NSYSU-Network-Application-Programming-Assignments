

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

/* Condition var for coordinating threads. */
static pthread_cond_t conditionVar = PTHREAD_COND_INITIALIZER;


/***************************************************/
void push(char oneChar) {
/***************************************************
Push a character onto the stack. Return in the
second argument the stack index corresponding to
where the character is pushed.
/***************************************************/
char string[25];

/* Get the lock before accessing the shared data
in any way. */
pthread_mutex_lock(&mutex);


while (index == STACK_SIZE) {
printWithTime ("push sleeping...\n");
pthread_cond_wait(&conditionVar, &mutex);
}

/* Stack is pushable. Push the data. */
buffer[index++] = oneChar;

sprintf (string,"Pushed:\tchar %c\tindex %d\n",
oneChar, index-1);
printWithTime(string);

/* Notify waiting threads (poppers in this case)
that the stack has changed and, due to the
operation just completed, there is now
something to pop. */
pthread_cond_signal(&conditionVar);

/* Release the mutex. All done with shared data
access. */
pthread_mutex_unlock(&mutex);
}

/***************************************************/
char pop() {
/***********************************************
Pop a character from the stack. Return in the
second argument the stack index corresponding to
where the character is popped.
/***************************************************/
char toReturn;
char string[25];

/* Get the lock before accessing the shared data
in any way. */
pthread_mutex_lock(&mutex);


while (index == 0) {
printWithTime ("pop sleeping...\n");
pthread_cond_wait(&conditionVar, &mutex);
}

/* Stack is poppable. Pop the data. */
toReturn = buffer[--index];

sprintf (string, "Pop:\tchar %c\tindex %d\n",
toReturn, index);
printWithTime(string);

/* Notify waiting threads (pushers in this case)
that the stack has changed and, due to the
operation just completed, there is now something
to push. */
pthread_cond_signal(&conditionVar);

/* Release the mutex. All done with shared data
access. */
pthread_mutex_unlock(&mutex);

return toReturn;
}