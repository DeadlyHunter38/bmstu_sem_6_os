#ifndef _READ_H_
#define _READ_H_

#include <unistd.h>

void read_file(char *path_to_open, char *file_out, char *file_mode, char *buffer);
void read_link(char *path_to_open, char *file_out, char *file_mode, char *buffer);

#endif