/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char * token;
  	char **newArgv;

  	/* Nothing entered. */
  	if (line == NULL) {
    	return NULL;
  	}

  	/* Init strtok with commandline, then get first token.
     * Return NULL if no tokens in line.
	 *
	 * Fill in code.
     */
	token = strtok(line, delim);
	if (token == NULL) {
		return NULL;
	}


  	/* Create array with room for first token.
  	 *
	 * Fill in code.
	 */
	newArgv = malloc(2 * sizeof(char *));
	if (newArgv == NULL) {
		return NULL;
	}
	newArgv[0] = malloc(strlen(token) + 1);
	if (newArgv[0] == NULL) {
		free(newArgv);
		return NULL;
	}
	strcpy(newArgv[0], token);


  	/* While there are more tokens...
	 *
	 *  - Get next token.
	 *	- Resize array.
	 *  - Give token its own memory, then install it.
	 * 
  	 * Fill in code.
	 */
	while ((token = strtok(NULL, delim)) != NULL) {
		count++;
		char **temp = realloc(newArgv, (count + 2) * sizeof(char *));
		if (temp == NULL) {
			free_argv(newArgv);
			return NULL;
		}
		newArgv = temp;
		newArgv[count] = malloc(strlen(token) + 1);
		if (newArgv[count] == NULL) {
			free_argv(newArgv);
			return NULL;
		}
		strcpy(newArgv[count], token);
	}


  	/* Null terminate the array and return it.
	 *
  	 * Fill in code.
	 */
	newArgv[count + 1] = NULL;
  	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv) {

	int i = 0;

	/* Free each string hanging off the array.
	 * Free the oldArgv array itself.
	 *
	 * Fill in code.
	 */
	while (oldArgv[i] != NULL) {
		free(oldArgv[i]);
		i++;
	}
	free(oldArgv);
}
