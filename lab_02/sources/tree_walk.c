#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../includes/tree_walk.h"

int do_path(const char *path_name, My_func *func, int depth)
{
    struct stat statbuf; struct dirent *dirp;
    DIR *dp; 
    int error = 0;

    if (strcmp(path_name, ".") == 0 || strcmp(path_name, "..") == 0){
        return 0;
    }

    if (lstat(path_name, &statbuf) == -1){
        /* ошибка вызова lstat */
        return (func(path_name, &statbuf, FTW_NS, depth));
    }

    if (S_ISDIR(statbuf.st_mode) == 0){
        /* файл */
        return (func(path_name, &statbuf, FTW_F, depth));
    }

    if ((error = func(path_name, &statbuf, FTW_D, depth)) != 0){
        /* не директория */
        return error;
    }

    if ((dp = opendir(path_name)) == NULL){
        /* каталог недоступен*/
        return (func(path_name, &statbuf, FTW_DNR, depth));
    }
    
    chdir(path_name);

    while ((dirp = readdir(dp)) != NULL && error == 0){
        error = do_path(dirp->d_name, func, depth + 1);
    }

    chdir("..");

    if (closedir(dp) == -1){
        fprintf(stderr, "Невозможно закрыть каталог(%s): %d.\n", path_name, -1);
    }

    return error;
}

int my_func(const char *path_name, const struct stat *stat_ptr, int type, int depth)
{
    //printf("my_func.\n");
    for (int i = 0; i < depth; i++){
        printf("|   ");
    }

    switch(type)
    {
        case FTW_F:
            switch (stat_ptr->st_mode & S_IFMT){
                case S_IFDIR:
                    fprintf(stderr, "Каталоги должны иметь тип FTW_D.\n");
                    return -1;
            }
        case FTW_D:
            printf("├── %s, %lu\n", path_name, stat_ptr->st_ino);
            break;
        case FTW_NS:
            fprintf(stderr, "Ошибка вызова для функции stat для %s.\n", path_name);
            return -1;
        default:
            fprintf(stderr, "Неизвесный тип %d для файла %s.\n", type, path_name);
            return -1;   
    }
    return 0;
}