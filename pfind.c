/*
 * pfind.c
 * Copyright (C) 2013 Tudor Marcu. All rights reserved.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <gmp.h>

FILE *create_file(char *filename)
{
	FILE *fd;
	char c;
	struct stat fstat;

	/* Check if file exists */
	if ((stat(filename, &fstat) == -1 && errno == ENOENT)) {
		fd = fopen(filename, "w");
	} else {
		printf("File already exists, would you like to replace? y/[n]");
		c = getchar();
		if (c == 'y')
			fd = fopen(filename, "w");
		else {
			printf("Exiting without creating file...\n");
			exit(EXIT_FAILURE);
		}
	}

	return fd;
}

void create_binary(char *binstr, unsigned int mem)
{
	unsigned int i;
	for (i = 0; i < mem/2; i++)
		binstr[i] = '1';

	for (i = mem/2; i < mem-1; i++)
		binstr[i] = '0';
}

void compute_perfect(mpz_t num)
{
	mpz_t i, q, sum;

	mpz_init(q);
	mpz_init_set_ui(i, 2);
	mpz_init_set_ui(sum, 1);

	mpz_out_str(stdout, 10, num);
	mpz_mod_ui(q, num, 2);

	if (mpz_get_ui(q) != 0)
		printf(" ISN'T PERFECT\n");

	while (mpz_cmp(i, num) < 0) {
		mpz_mod(q, num, i);
		if (mpz_cmp_ui(q, 0) == 0) 
			mpz_add(sum, sum, i);

		mpz_add_ui(i, i, 1);
	}

	if (mpz_cmp(sum, num) == 0)
		printf(" IS PERFECT\n");
	else
		printf(" ISN'T PERFECT\n");

	mpz_clear(i);
	mpz_clear(q);
	mpz_clear(sum);
}

int main(int argc, char *argv[])
{
	unsigned int reqmem;
	int checknum = 0;
	char *numstr;
	char *filename;
	mpz_t x;
	FILE *output;

	if (argc < 3) {
		fprintf(stderr, "Usage: pfind [prime-number] [filename] \n\t"
				"-c  Optional brute force end number check\n");
		exit(EXIT_FAILURE);
	}
	if (argc > 3) {
		if (strcmp(argv[3], "-c") == 0)
			checknum = 1;
		else
			fprintf(stderr, "%s incorrect, ignoring...\n", argv[3]);
	}

	mpz_init(x);

	if (isdigit(*argv[1]) == 0) {
		fprintf(stderr, "Please enter a numeric value for prime...\n");
		exit(EXIT_FAILURE);
	}
	reqmem = (2 * atoi(argv[1]));
	filename = strdup(argv[2]);

	numstr = malloc(reqmem); /* allocate memory for binary representation*/
	if (numstr == NULL) {
		fprintf(stderr, "Failed to allocate memory for binary.\n");
		exit(EXIT_FAILURE);
	}

	output = create_file(filename);
	if (output == NULL) {
		fprintf(stderr, "Could not open file\n");
		exit(EXIT_FAILURE);
	}

	create_binary(numstr, reqmem);

	mpz_set_str(x, numstr, 2);
	mpz_out_str(output, 10, x);
	fprintf(output, "\n");
	if (checknum == 1)
		compute_perfect(x);

	fclose(output);
	mpz_clear(x);
	free(numstr);

	return EXIT_SUCCESS;
}
