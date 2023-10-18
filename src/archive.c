#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "archive.h"
#include "util.h"
#include "types.h"

#define MAGIC_STRING "EZZ"
#define PATH_SIZE 256

void
extract_archive (const char *path)
{
	FILE *archive = fopen(path, "rb");
	if (!archive) exit_with_error("Failed to read archive.");

	char MAGIC[4];
	fread(MAGIC, sizeof(char), 4, archive);

	if (strcmp(MAGIC, MAGIC_STRING) != 0)
		exit_with_error("File is not an EZZ archive.");

	while (!feof(archive))
	{
		char entry_path[PATH_SIZE];
		fread(entry_path, sizeof(char), PATH_SIZE, archive);

		unsigned long fsize;
		fread(&fsize, sizeof(unsigned long), 1, archive);
		if (fsize == 0) break;

		char base[strlen(entry_path)+1];
		strcpy(base, entry_path);
		get_base_dir(base);

		char *current_dir = calloc(strlen(entry_path), sizeof(char));
		char *ptr = strtok(base, "/");
		while (ptr != NULL)
		{
			strcat(current_dir, ptr);
			strcat(current_dir, "/");

			struct stat status = { 0 };
			if (stat(current_dir, &status) == -1)
				mkdir(current_dir, 0700);

			ptr = strtok(NULL, "/");
		}
		printf("Extracting: %s | %lu BYTES\n", entry_path, fsize);
		free(current_dir);

		FILE *ext_file = fopen(entry_path, "wb");
		if (!ext_file) exit_with_error("Failed to extract file.");

		char buffer[1024*1024];
		while (fsize > 0)
		{
			unsigned long available_bytes = (fsize > sizeof(buffer)) ? sizeof(buffer) : fsize;
			size_t bytes_read = fread(buffer, 1, available_bytes, archive);
			if (bytes_read == 0)
			{
				if (feof(archive)) exit_with_error("Reached end of archive.");
				else exit_with_error("Couldn't read from archive.");
			}
			fwrite(buffer, bytes_read, 1, ext_file);
			fsize -= bytes_read;
		}
		fclose(ext_file);
	}
	fclose(archive);
}

static void
write_entry (const char *path, FILE *output_file)
{
	static unsigned long total = 0; /* total bytes archived */
	FILE *target_file = fopen(path, "rb");
	if (!target_file) exit_with_error("Failed to open path.");

	unsigned long fsize = get_file_size(target_file);
	total += (fsize+PATH_SIZE+sizeof(unsigned long));

	fwrite(path, sizeof(char), PATH_SIZE, output_file);
	printf("Adding: %s (%lu bytes) | Total: [%lu B]\n",
		path, fsize, total);

	fwrite(&fsize, sizeof(unsigned long), 1, output_file);

	/* TODO:
	 * Make this a buffered read
	 * */
	char *data = malloc(fsize);
	if (!data) exit_with_error("Not enough RAM to archive file.");
	fread(data, sizeof(char), fsize, target_file);

	fwrite(data, sizeof(char), fsize, output_file);
	fclose(target_file);
	free(data);
}

void
create_archive (const char *basedir, FILE *output_file)
{
	DIR *dir = opendir(basedir);
	if (!dir) exit_with_error("Can't archive directory.");

	char path[PATH_SIZE];
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
			create_archive(path, output_file);
		else
			write_entry(path, output_file);
	}
	closedir(dir);
}
