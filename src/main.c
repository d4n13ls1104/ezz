#include <stdlib.h>
#include <string.h>
#include "archive.h"
#include "util.h"

int
main (int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Invalid arguments.\n");
		exit(EXIT_FAILURE);
	}
	char *output_path = argv[1];

	char *path = argv[2];
	if (path[strlen(path) - 1] == '/')
		path[strlen(path) - 1] = 0;

	create_archive(path, output_path);
	return 0;
}
