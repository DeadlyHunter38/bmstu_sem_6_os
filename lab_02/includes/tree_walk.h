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

typedef int My_func(const char *, const struct stat *, int, int);

My_func my_func;
int do_path(const char *, My_func *, int);

#endif