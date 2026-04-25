#include <signal.h>
#include <stddef.h>
#include "SystemVsigs.h"

void (*sigset(int sig, void (*handler)(int)))(int)
{
    struct sigaction sa, oldsa;
    sigset_t set, oldset;
    int was_blocked;

    sigemptyset(&set);
    sigaddset(&set, sig);

    if (handler == SIG_HOLD)
    {
        if (sigprocmask(SIG_BLOCK, &set, &oldset) == -1)
            return SIG_ERR;

        was_blocked = sigismember(&oldset, sig);
        if (was_blocked == -1)
            return SIG_ERR;

        if (sigaction(sig, NULL, &oldsa) == -1)
            return SIG_ERR;

        // if the signal was already blocked before, we return SIG_HOLD to indicate that the signal is still blocked, even though we didn't change the handler.
        if (was_blocked)
            return SIG_HOLD;

        return oldsa.sa_handler;
    }

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(sig, &sa, &oldsa) == -1)
        return SIG_ERR;

    if (sigprocmask(SIG_UNBLOCK, &set, &oldset) == -1)
        return SIG_ERR;

    was_blocked = sigismember(&oldset, sig);
    if (was_blocked == -1)
        return SIG_ERR;

    if (was_blocked)
        return SIG_HOLD;

    return oldsa.sa_handler;
}

int sighold(int sig)
{
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, sig);

    return sigprocmask(SIG_BLOCK, &set, NULL);
}

int sigrelse(int sig)
{
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, sig);

    return sigprocmask(SIG_UNBLOCK, &set, NULL);
}

int sigignore(int sig)
{
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    return sigaction(sig, &sa, NULL);
}

int sigpause(int sig)
{
    sigset_t mask;

    sigprocmask(SIG_SETMASK, NULL, &mask); 
    sigdelset(&mask, sig);                 

    return sigsuspend(&mask);
}