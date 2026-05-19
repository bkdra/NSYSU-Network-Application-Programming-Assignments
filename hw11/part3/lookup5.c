/*
 * lookup5 : local file ; setup a memory map of the file
 *           Use bsearch. We assume that the words are already
 *           in dictionary order in a file of fixed-size records
 *           of type Dictrec
 *           The name of the file is what is passed as resource
 */

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dict.h"

/*
 * This obscure looking function will be useful with bsearch
 * It compares a word with the word part of a Dictrec
 */

int dict_cmp(const void *a,const void *b) {
    return strcmp((char *)a,((Dictrec *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Dictrec * table;
	static int numrec;
	Dictrec * found;
	static int first_time = 1;

	if (first_time) {  /* table ends up pointing at mmap of file */
		int fd;
		long filsiz;

		first_time = 0;

		/* Open the dictionary.
		 * Fill in code. */
		fd = open(resource,O_RDONLY);
		if (fd < 0) {
			perror(resource);
			exit(1);
		}

		/* Get record count for building the tree. */
		filsiz = lseek(fd,0L,SEEK_END);		//calculate file size
		numrec = filsiz / sizeof(Dictrec); // calculate number of records in the file

		/* mmap the data.
		 * Fill in code. */
		table = mmap(NULL, filsiz, PROT_READ, MAP_PRIVATE, fd, 0);
		if (table == MAP_FAILED) {
			perror("mmap");
			exit(1);
		}
		close(fd);
	}
    
	/* search table using bsearch
	 * Fill in code. */
	found = bsearch(sought->word, table, numrec, sizeof(Dictrec), dict_cmp);
	if (found) {
		strcpy(sought->text,found->text);
		return FOUND;
	}

	return NOTFOUND;
}
