#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <stdio.h>

void create_archive 	(const char *basedir, FILE *output_file);
void extract_archive 	(const char *path);

#endif
