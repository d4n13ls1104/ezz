#include "util.h"
#include <stdlib.h>
#include <string.h>

void
trim (char *str, char c)
{
  unsigned long i = strlen (str) - 1;
  if (str[i] == c)
    str[i] = 0;
}

unsigned long
get_file_size (FILE *f)
{
  fseek (f, 0, SEEK_END);
  unsigned long size = ftell (f);
  rewind (f);
  return size;
}

void
get_path_base (char *path)
{
  int i;
  for (i = strlen (path); i > 0; i--)
    if (path[i] == '/')
      {
        path[i] = 0;
        break;
      }
}
