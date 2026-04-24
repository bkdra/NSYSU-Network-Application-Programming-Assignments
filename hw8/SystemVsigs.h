#define _XOPEN_SOURCE 500

#include <signal.h>

#ifndef SIG_HOLD
#define SIG_HOLD ((void (*)(int))2)
#endif

void (*sigset(int sig, void (*handler)(int)))(int);
int sighold(int sig);
int sigrelse(int sig);
int sigignore(int sig);
int sigpause(int sig);