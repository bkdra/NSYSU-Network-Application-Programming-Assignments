/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"

void run_command(char **myArgv) {
    pid_t pid;
    int stat;

    /* Create a new child process.
     * Fill in code.
	 */
    int isBackground = is_background(myArgv);
    pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
            /* Wait for child to terminate.
             * Fill in code.
			 */
            if(!isBackground)
            {
                if(waitpid(pid, &stat, 0) == -1) {
                perror("waitpid");
                exit(errno);
                }
                /* Optional: display exit status.  (See wstat(5).)
                * Fill in code.
                */
                if(WIFEXITED(stat)) {
                    printf("Child exited with status %d\n", WEXITSTATUS(stat));
                } else if (WIFSIGNALED(stat)) {
                    printf("Child killed by signal %d\n", WTERMSIG(stat));
                } else if (WIFSTOPPED(stat)) {
                    printf("Child stopped by signal %d\n", WSTOPSIG(stat));
                }
            }
            
            return;

        /* Child. */
        case 0 :
            /* Run command in child process.
             * Fill in code.
			 */
            execvp(myArgv[0], myArgv);
            perror("execvp");
            /* Handle error return from exec */
			exit(errno);
    }
}
