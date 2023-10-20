#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "archive.h"
#include "util.h"
#include "types.h"

#define PATH_SIZE 256
#define BUFFER_SIZE 1024*1024
#define MAGIC_STRING "EZZ"

static void
fcopy (FILE *dest, FILE *src, unsigned long fsize)
{
	char buffer[BUFFER_SIZE];
	unsigned long available_bytes, bytes_read = 0;
	while (!feof(src) && fsize > 0)
	{
		available_bytes = (fsize > BUFFER_SIZE) ? BUFFER_SIZE : fsize;
		bytes_read = fread(buffer, sizeof(char), available_bytes, src);
		if (bytes_read == 0) exit_with_error("Failed to read src.");

		fwrite(buffer, sizeof(char), bytes_read, dest);
		fsize -= bytes_read;
	}
}

static void
write_entry (const char *path, FILE *output_file)
{
	FILE *target_file = fopen(path, "rb");
	if (!target_file) exit_with_error("Failed to add file.");

	fwrite(path, sizeof(char), PATH_SIZE, output_file);
	printf("%s\n", path);

	unsigned long fsize = get_file_size(target_file);
	fwrite(&fsize, sizeof(unsigned long), 1, output_file);

	fcopy(output_file, target_file, fsize);
	fclose(target_file);
}

void
create_archive (const char *basedir, FILE *output_file)
{
	DIR *dir = opendir(basedir);
	if (!dir) exit_with_error("Couldn't archive directory.");

	char path[PATH_SIZE];
	struct dirent *entity;
	while ((entity = readdir(dir)))
	{
		if (strcmp(entity->d_name, ".") == 0 ||
		strcmp(entity->d_name, "..") == 0) continue;

		strcpy(path, basedir);
		strcat(path, "/");
		strcat(path, entity->d_name);

		if (entity->d_type == DT_DIR)
			create_archive(path, output_file);
		else
			write_entry(path, output_file);
	}
	closedir(dir);
}

void
extract_archive (const char *path)
{
	FILE *archive = fopen(path, "rb");
	if (!archive) exit_with_error("Failed to open archive.");

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

		char base[strlen(entry_path) + 1];
		strcpy(base, entry_path);
		get_path_base(base);

		char current_dir[PATH_SIZE] = { 0 };
		char *dptr = strtok(base, "/");
		while (dptr)
		{
			strcat(current_dir, dptr);
			strcat(current_dir, "/");

			struct stat status = { 0 };
			if (stat(current_dir, &status) == -1)
				mkdir(current_dir, 0700);

			dptr = strtok(NULL, "/");
		}
		printf("%s\n", entry_path);

		FILE *ext_file = fopen(entry_path, "ab");
		if (!ext_file) exit_with_error("Couldn't extract file.");

		fcopy(ext_file, archive, fsize);
		fclose(ext_file);
	}
	fclose(archive);
}
