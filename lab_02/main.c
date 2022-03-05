#include <stdio.h>
#include "./includes/tree_walk.h"

int main(int argc, char *argv[])
{
    if (argc != 2){
        perror("Некорректное число аргументов (имя программы, имя каталога)");
        return -1;
    }

    int error = do_path(argv[1], func, 0);
    return error;
}