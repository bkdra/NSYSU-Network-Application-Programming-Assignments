/*
 * is_background.c :  check for & at end
 */


#include <stdio.h>
#include <string.h>
#include "shell.h"

int is_background(char ** myArgv) {

  	if (*myArgv == NULL)
    	return 0;

  	/* Look for "&" in myArgv, and process it.
  	 *
	 *	- Return TRUE if found.
	 *	- Return FALSE if not found.
	 *
	 * Fill in code.
	 */
	int i = 0;
	int isBackground = 0;
	while (myArgv[i] != NULL) {
		if (!isBackground && strcmp(myArgv[i], "&") == 0) {
			myArgv[i] = NULL; /* Remove "&" from argv. */
			isBackground = 1;
			printf("Running in background...\n");
		}
		else if(isBackground)
		{
			myArgv[i-1] = myArgv[i]; /* Shift arguments left to fill in the gap. */
			myArgv[i] = NULL;
		}
		i++;
	}
	return isBackground;
}