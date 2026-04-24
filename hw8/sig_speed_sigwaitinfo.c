/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2026.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 22 */

#include <signal.h>
#include "tlpi_hdr.h"
#include <time.h>


#define TESTSIG SIGUSR1

int
main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s num-sigs\n", argv[0]);

    int numSigs = getInt(argv[1], GN_GT_0, "num-sigs");

    /* Block the signal before fork(), so that the child doesn't manage
       to send it to the parent before the parent is ready to catch it */

    sigset_t testsigMask, emptyMask;
    sigemptyset(&testsigMask);
    sigaddset(&testsigMask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &testsigMask, NULL) == -1)
        errExit("sigprocmask");

    sigemptyset(&emptyMask);

    pid_t childPid = fork();
    switch (childPid) {
    case -1: errExit("fork");

    case 0:     /* child */
        for (int scnt = 0; scnt < numSigs; scnt++) {
            if (kill(getppid(), TESTSIG) == -1)
                errExit("kill");
            if (sigwaitinfo(&testsigMask, NULL) == -1 && errno != EINTR)
                    errExit("sigwaitinfo");
        }
        exit(EXIT_SUCCESS);

    default: /* parent */
        struct timespec t0, t1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int scnt = 0; scnt < numSigs; scnt++) {
            if (sigwaitinfo(&testsigMask, NULL) == -1 && errno != EINTR)
                    errExit("sigwaitinfo");
            if (kill(childPid, TESTSIG) == -1)
                errExit("kill");
        }
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed =
        (double)(t1.tv_sec - t0.tv_sec) +
        (double)(t1.tv_nsec - t0.tv_nsec) / 1e9;

        printf("Time taken: %.6f seconds\n", elapsed);
        exit(EXIT_SUCCESS);
    }
}
