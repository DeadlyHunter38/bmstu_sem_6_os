#ifndef _TREE_WALK_
#define _TREE_WALK_

#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

#define FTW_F 1 /* файл, не являющийся каталогом */
#define FTW_D 2 /* каталог */
#define FTW_DNR 3 /* каталог, недоступный для чтения */
#define FTW_NS 4 /* файл, информацию о котором невозможно получить с помощью stat */

int func(const char *path_name, const struct stat *stat_ptr, int type, int depth);
int do_path(const char *path_name, int (*func)(const char *, const struct stat *, int, int), int depth);

#endif