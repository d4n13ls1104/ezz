#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>

void 		    	trim            (char *str, char c);
void 		    	get_path_base 	(char *path);
unsigned long 		get_file_size 	(FILE *f);

#endif
