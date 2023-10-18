#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>

void trim 			(char *str, char c);
void exit_with_error 		(const char *message);
void get_base_dir 		(char *path);
unsigned long get_file_size 	(FILE *f);

#endif
