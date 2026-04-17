/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"




/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/

/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_echo(char **argv) {
  	/* Fill in code. */
	int i = 1;
	int newline = 1; /* Whether to print final newline. */
	if (argv[1] != NULL && strcmp(argv[1], "-n") == 0) {
		newline = 0;
		i = 2;
	}
	while (argv[i] != NULL) {
		printf("%s", argv[i]);
		if (argv[i + 1] != NULL) {
			printf(" ");
		}
		i++;
	}
	if (newline) {
		printf("\n");
	}
}

static void bi_cd(char **argv) {
  	/* Fill in code. */
	if (argv[1] == NULL) {
		fprintf(stderr, "cd: missing argument\n");
		return;
	}
	if (chdir(argv[1]) == -1) {
		perror("cd");
	}
}

static void bi_exit(char **argv) {
  	/* Fill in code. */
	exit(0);
}

static void bi_pwd(char **argv) {
  	/* Fill in code. */
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("pwd");
	} else {
		printf("%s\n", cwd);
	}
}


/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
	char * keyword;				/* When this field is argv[0] ... */
	void (* do_it)(char **);	/* ... this function is executed. */
} inbuilts[] = {

	/* Fill in code. */
	{ "cd", bi_cd },			/* When "cd" is typed, bi_cd() executes.  */
	{ "exit", bi_exit },		/* When "exit" is typed, bi_exit() executes.  */
	{ "pwd", bi_pwd },			/* When "pwd" is typed, bi_pwd() executes.  */
	{ "echo", bi_echo },		/* When "echo" is typed, bi_echo() executes.  */
	{ NULL, NULL }				/* NULL terminated. */
};




/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; 		/* close coupling between is_builtin & do_builtin */

/* Check to see if command is in the inbuilts table above.
Hold handle to it if it is. */
int is_builtin(char *cmd) {
  	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,cmd) == 0) {
			this = tableCommand;
			return 1;
		}
  	return 0;
}


/* Execute the function corresponding to the builtin cmd found by is_builtin. */
int do_builtin(char **argv) {
  	this->do_it(argv);
}
