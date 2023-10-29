/*
 * EXAMPLE USAGE
 * ezz c Archive.EZZ Archive/
 * ezz x Archive.EZZ
 *
 */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archive.h"
#include "util.h"

#define OP_CREATE 	'c'
#define OP_EXTRACT 	'x'
#define MAGIC "EZZ"

int
main (int argc, char *argv[])
{
  if (argc <= 2)
    errx (EXIT_FAILURE, "Invalid arguments.\n");
  switch (*argv[1])
    {
    case OP_CREATE:
      {
        if (argc != 4)
          errx (EXIT_FAILURE, "Usage: %s %c Archive.EZZ Archive/\n", argv[0],
                OP_CREATE);
        char *output_path = argv[2];
        char *dirpath = argv[3];
        trim (dirpath, '/');

        FILE *output_file = fopen (output_path, "wb");
        if (!output_file)
          errx (EXIT_FAILURE, "Failed to open file: %s\n", output_path);
        fwrite (MAGIC, sizeof (char), 4, output_file);

        create_archive (dirpath, output_file);
        fclose (output_file);
        break;
      }
    case OP_EXTRACT:
      {
        if (argc != 3)
          errx (EXIT_FAILURE, "Usage: %s %c Archive.EZZ\n", argv[0],
                OP_EXTRACT);

        char *archive_path = argv[2];
        trim (archive_path, '/');
        extract_archive (archive_path);
        break;
      }
    default:
      errx (EXIT_FAILURE, "Unrecognized operation.");
    }
  return 0;
}
