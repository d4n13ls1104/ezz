#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "archive.h"
#include "types.h"
#include "util.h"

#define PATH_SIZE     256
#define BUFFER_SIZE   1024 * 1024
#define MAGIC_STRING  "EZZ"

static void
fcopy (FILE *dest, FILE *src, unsigned long fsize)
{
  char buffer[BUFFER_SIZE];
  unsigned long available_bytes, bytes_read;
  while (!feof (src) && fsize > 0)
    {
      available_bytes = (fsize > BUFFER_SIZE) ? BUFFER_SIZE : fsize;
      bytes_read = fread (buffer, sizeof (char), available_bytes, src);
      if (bytes_read == 0)
        errx (EXIT_FAILURE, "fcopy: failed to read source file\n");

      fwrite (buffer, sizeof (char), bytes_read, dest);
      fsize -= bytes_read;
    }
}

static void
write_entry (const char *path, FILE *output_file)
{
  FILE *target_file = fopen (path, "rb");
  if (!target_file)
    errx (EXIT_FAILURE, "Failed to add entry: %s\n", path);

  unsigned long fsize = get_file_size (target_file);
  if (fsize == 0)
    return;

  fwrite (path, sizeof (char), PATH_SIZE, output_file);
  fwrite (&fsize, sizeof (unsigned long), 1, output_file);
  fcopy (output_file, target_file, fsize);
  fclose (target_file);
  printf ("%s\n", path);
}

void
create_archive (const char *basedir, FILE *output_file)
{
  DIR *dir = opendir (basedir);
  if (!dir)
    errx (EXIT_FAILURE, "Couldn't open directory: %s\n", basedir);

  char path[PATH_SIZE];
  struct dirent *entity;
  while ((entity = readdir (dir)))
    {
      if (strcmp (entity->d_name, ".") == 0
          || strcmp (entity->d_name, "..") == 0)
        continue;

      strncpy (path, basedir, PATH_SIZE);
      strncat (path, "/", PATH_SIZE - strlen (path) - 1);
      strncat (path, entity->d_name, PATH_SIZE - strlen (path) - 1);
      path[PATH_SIZE - 1] = '\0';

      if (entity->d_type == DT_DIR)
        create_archive (path, output_file);
      else
        write_entry (path, output_file);
    }
  closedir (dir);
}

void
extract_archive (const char *path)
{
  FILE *archive = fopen (path, "rb");
  if (!archive)
    errx (EXIT_FAILURE, "Failed to open archive: %s\n", path);

  char MAGIC[4];
  fread (MAGIC, sizeof (char), 4, archive);

  if (strcmp (MAGIC, MAGIC_STRING) != 0)
    errx (EXIT_FAILURE, "File is not an EZZ archive.\n");

  while (!feof (archive))
    {
      char entry_path[PATH_SIZE];
      fread (entry_path, sizeof (char), PATH_SIZE, archive);

      unsigned long fsize;
      fread (&fsize, sizeof (unsigned long), 1, archive);
      if (fsize == 0)
        continue;

      char base[PATH_SIZE];
      strcpy (base, entry_path);
      get_path_base (base);

      char current_dir[PATH_SIZE] = { 0 };
      char *dptr = strtok (base, "/");
      while (dptr)
        {
          strncat (current_dir, dptr, PATH_SIZE - strlen (current_dir) - 1);
          strncat (current_dir, "/", PATH_SIZE - strlen (current_dir) - 1);

          struct stat status = { 0 };
          if (stat (current_dir, &status) == -1)
            mkdir (current_dir, 0700);

          dptr = strtok (NULL, "/");
        }
      printf ("%s\n", entry_path);

      FILE *ext_file = fopen (entry_path, "ab");
      if (!ext_file)
        errx (EXIT_FAILURE, "Couldn't extract file: %s\n", entry_path);

      fcopy (ext_file, archive, fsize);
      fclose (ext_file);
    }
  fclose (archive);
}
