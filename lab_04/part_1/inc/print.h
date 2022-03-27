#ifndef _PRINT_H_
#define _PRINT_H_

#include <stdio.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define _GNU_SOURCE
#include <dirent.h>

#define FTW_F 1 /* файл, не являющийся каталогом */
#define FTW_D 2 /* каталог */
#define FTW_DNR 3 /* каталог, недоступный для чтения */
#define FTW_NS 4 /* файл, информацию о котором невозможно получить с помощью stat */

int get_pid(int argc, char *argv[]);
int print_environ(const int pid);
int print_cmdline(const int pid);
int print_cwd(const int pid);
void print_exe(const int pid);
void print_fd(const int pid);
void print_root(const int pid);
void print_stat(const int pid);
void print_maps(const int pid);
void print_io(const int pid);
void print_comm(const int pid);
void print_task(const int pid);

#endif