/* 
 * EXAMPLE USAGE 
 * ezz c Archive.EZZ Archive/
 * ezz x Archive.EZZ
 *
*/
#include <stdio.h>
#include <string.h>
#include "archive.h"
#include "util.h"

#define OP_CREATE 	'c'
#define OP_EXTRACT 	'x'

#define MAGIC "EZZ"

int
main (int argc, char *argv[])
{
	if (argc <= 2) exit_with_error("Invalid arguments.");
	switch (*argv[1])
	{
		case OP_CREATE:
			{
				if (argc != 4) exit_with_error("Invalid arguments.");
				char *output_path = argv[2];
				char *dirpath = argv[3];
				trim(dirpath, '/');

				FILE *output_file = fopen(output_path, "wb");
				if (!output_file) exit_with_error("Couldn't open output file.");
				fwrite(MAGIC, sizeof(char), 4, output_file);

				create_archive(dirpath, output_file);
				fclose(output_file);
				break;
			}
		case OP_EXTRACT:
			{
				if (argc != 3) exit_with_error("Invalid arguments.");
				char *archive_path = argv[2];
				trim(archive_path, '/');

				extract_archive(archive_path);
				break;
			}
		default: exit_with_error("Unrecognized operation.");
	}
	return 0;
}
