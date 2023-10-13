#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "archive.h"
#include "util.h"
#include "types.h"

#define MAGIC_STRING "EZZ"
#define PATH_SIZE 256
#define EOF_PADDING_SIZE 1024

static void
write_entry (const char *path, FILE *output_file)
{
	static unsigned long total = 0; /* total bytes archived */
	FILE *target_file = fopen(path, "rb");
	if (!target_file) exit_with_error("Failed to open path.");

	unsigned long fsize = get_file_size(target_file);
	total += (fsize+PATH_SIZE+EOF_PADDING_SIZE);

	fwrite(path, PATH_SIZE, 1, output_file);
	printf("Adding: %s (%lu bytes) | Total: [%lu B]\n",
		path, fsize, total);

	char *data = malloc(fsize);
	if (!data) exit_with_error("Not enough RAM to archive file.");
	fread(data, fsize, 1, target_file);
	fwrite(data, fsize, 1, output_file);

	fclose(target_file);
	free(data);

	char offset_block[EOF_PADDING_SIZE] = { 0 };
	fwrite(offset_block, EOF_PADDING_SIZE, 1, output_file);
}

void
create_archive (const char *basedir, const char *outputdir)
{
	DIR *dir = opendir(basedir);
	if (!dir) exit_with_error("Can't open directory.");

	FILE *output_file = fopen(outputdir, "ab");
	if (!output_file) exit_with_error("Can't open output path.");

	fwrite(MAGIC_STRING, 4, 1, output_file);

	char *path = calloc(PATH_SIZE, 1);
	struct dirent *entity;
	while ((entity = readdir(dir)) != NULL)
	{
		if (strcmp(entity->d_name, ".") == 0 ||
		strcmp(entity->d_name, "..") == 0) continue;

		memset(path, 0, PATH_SIZE);
		strcat(path, basedir);
		strcat(path, "/");
		strcat(path, entity->d_name);

		if (entity->d_type == DT_DIR)
			create_archive(path, outputdir);
		else
			write_entry(path, output_file);
	}
	free(path);
	fclose(output_file);
	closedir(dir);
}
