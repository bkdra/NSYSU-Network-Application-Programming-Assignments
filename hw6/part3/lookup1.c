/*
 * lookup1 : straight linear search through a local file
 * 	         of fixed length records. The file name is passed
 *	         as resource.
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>	
#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	Dictrec dr;
	static FILE * in;
	static int first_time = 1;

	if (first_time) { 
		first_time = 0;
		/* open up the file
		 *
		 * Fill in code. */
		in = fopen(resource, "r");
		if (!in) {
			return NOTFOUND;
		}

	}

	/* read from top of file, looking for match
	 *
	 * Fill in code. */
	char tmp_word[WORD];
	for(int i=0; i<WORD; i++)
	{
		tmp_word[i] = ' ';
	}
	memcpy(tmp_word, sought->word, strlen(sought->word));
	tmp_word[WORD-1] = '\0';
	// printf("looking for word: %s\n", tmp_word);

	rewind(in);
	while(fread(dr.word, 1, WORD, in) != 0) {
		dr.word[WORD-1] = '\0';
		// printf("read word: %s\n", dr.word);
		if(fread(dr.text, 1, TEXT, in) == 0) {
				return NOTFOUND;
		}
		dr.text[TEXT-1] = '\0';
		// printf("read text: %s\n", dr.text);
		if (strcmp(dr.word, tmp_word) == 0) {
			char* token = strtok(dr.text, " \t\n");
			char* final_str = NULL;
			char *tmp;
			while(token != NULL)
			{
				tmp = malloc(strlen(token)+1);
				strcpy(tmp, token);
				if (!tmp) {
					return NOTFOUND;
				}
				strcat(tmp, " ");
				if(final_str == NULL)
				{
					final_str = malloc(strlen(tmp)+1);
					strcpy(final_str, tmp);
				}
				else
				{
					final_str = realloc(final_str, strlen(final_str) + strlen(tmp) + 1);
					strcat(final_str, tmp);
				}
				free(tmp);
				token = strtok(NULL, " \t\n");
			}

			strncpy(sought->text, final_str, strlen(final_str));
			return FOUND;
		}
	}

	return NOTFOUND;
}
