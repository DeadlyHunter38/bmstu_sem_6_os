/*
* Наш заголовочный файл, который подключается перед любыми
* стандартными системными заголовочными файлами
*/
#ifndef _APUE_H
#define _APUE_H
#define _POSIX_C_SOURCE 200809L
#if defined(SOLARIS)
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 700
#endif
/* Solaris 10 */
#include <sys/types.h>
/* некоторые системы требуют этого заголовка */
#include <sys/stat.h>
#include <sys/termios.h> /* структура winsize */
#if defined(MACOS) || !defined(TIOCGWINSZ)
