#include <stdlib.h>

#include "util.h"

void exit_with_error(const char *msg)
{
	printf("An error occured: %s\n", msg);
	exit(EXIT_FAILURE);
}

unsigned long get_file_size(FILE *f)
{
	fseek(f, 0, SEEK_END);
	unsigned long size = ftell(f);
	rewind(f);
	return size;
}
