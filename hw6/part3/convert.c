/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BIGLINE 512

int main(int argc, char **argv) {
	FILE *in;
	FILE *out;        /* defaults */
	char line[BIGLINE];
	static Dictrec dr, blank;
	
	/* If args are supplied, argv[1] is for input, argv[2] for output */
	if (argc==3) {
		if ((in =fopen(argv[1],"r")) == NULL){DIE(argv[1]);}
		if ((out =fopen(argv[2],"w")) == NULL){DIE(argv[2]);}	
	}
	else{
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	/* Main reading loop : read word first, then definition into dr */

	/* Loop through the whole file. */
	while (!feof(in)) {
		
		/* Create and fill in a new blank record.
		 * First get a word and put it in the word field, then get the definition
		 * and put it in the text field at the right offset.  Pad the unused chars
		 * in both fields with nulls.
		 */
		dr = blank;	/* Start with a blank record. */

		/* Read word and put in record.  Truncate at the end of the "word" field.
		 *
		 * Fill in code. */
		int n;
		if(fgets(line, sizeof(line), in) == NULL)
		{
			if(errno == 0)
			{
				break;
			}
			else
			{
				DIE("read");
			}
		}
		
		// line[n] = '\0';
		char *token = strtok(line, "\n");
		strncpy(dr.word, token, strlen(token));
		// printf("word: %s\n", dr.word);

		/* Read definition, line by line, and put in record.
		 *
		 * Fill in code. */
		while((fgets(line, sizeof(line), in) != NULL) && (strcmp(line, "\n") != 0))
		{
			
			if(strcmp(line, "\n") == 0)
			{
				break;
			}
			strncat(dr.text, line, TEXT - strlen(dr.text) - 1);
		}
		// printf("text: %s\n", dr.text);

		/* Write record out to file.
		 *
		 * Fill in code. */
		int num = fwrite(dr.word, sizeof(char), strlen(dr.word), out);
		while(num < WORD)
		{
			if(num == -1)
			{
				DIE("fwrite");
			}
			num += fwrite(" ", sizeof(char), 1, out);
		}
		num = fwrite(dr.text, sizeof(char), strlen(dr.text), out);
		while(num < TEXT)
		{
			if(num == -1)
			{
				DIE("fwrite");
			}
			num += fwrite(" ", sizeof(char), 1, out);
		}
	}

	fclose(in);
	fclose(out);
	return 0;
}
