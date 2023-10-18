#include <stdlib.h>
#include <string.h>
#include "util.h"

void
trim (char *str, char c)
{
	unsigned long i = strlen(str)-1;
	if (str[i] == c) str[i] = 0;
}

void
exit_with_error (const char *msg)
{
	printf("An error occured: %s\n", msg);
	exit(EXIT_FAILURE);
}

unsigned long
get_file_size (FILE *f)
{
	fseek(f, 0, SEEK_END);
	unsigned long size = ftell(f);
	rewind(f);
	return size;
}

void
get_base_dir (char *path)
{
	int i;
	for (i = strlen(path); i > 0; i--)
		if (path[i] == '/')
		{
			path[i] = 0;
			break;
		}
}
