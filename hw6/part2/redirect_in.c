/*
 * redirect_in.c  :  check for <
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/*
 * Look for "<" in myArgv, then redirect input to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_in(char ** myArgv) {
  	int i = 0;
  	int fd;

  	/* search forward for <
  	 *
	 * Fill in code. */
	while(myArgv[i] != NULL)
	{
		if (strcmp(myArgv[i], "<") == 0) {	/* found "<" in vector. */

			/* 1) Open file.
			* 2) Redirect stdin to use file for input.
			* 3) Cleanup / close unneeded file descriptors.
			* 4) Remove the "<" and the filename from myArgv.
			*
			* Fill in code. */

			fd = open(myArgv[i+1], O_RDONLY );
			if (fd == -1) {
				perror("redirect_in: open");
				return -1;
			}
			if (dup2(fd, STD_INPUT) == -1) {
				perror("redirect_in: dup2");
				close(fd);
				return -1;
			}
			close(fd);
			/* Remove the "<" and the filename from myArgv. */
			int j = i;
			free(myArgv[i]);
			free(myArgv[i+1]);
			while(myArgv[j+2] != NULL)
			{
				myArgv[j] = myArgv[j+2];
				j++;
			}
			myArgv[j] = NULL;
		}
		i++;
	}
		
  	return 0;
}
