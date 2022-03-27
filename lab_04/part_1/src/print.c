#include "../inc/print.h"
#include "../inc/read.h"
#include "../inc/const.h"
#include "../inc/stat.h"

int do_path(const char *path_name, int (*func)(const char *, const struct stat *, int, int), int depth,
            FILE *file, FILE *file_log);
int func_tree(const char *path_name, const struct stat *stat_ptr, int type,
              int depth, FILE *file, FILE *file_log);

int get_pid(int argc, char *argv[])
{
    if (argc < 2){
        perror("Ошибка: некорректное число аргументов (!= 2).");
        return -1;
    }

    int pid = atoi(argv[1]);
    return pid;
}

int print_cmdline(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/cmdline", pid);
        
    read_file(path_to_open, "log.txt", "w", buffer);   
}

int print_cwd(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE] = {'\0'};
    
    snprintf(path_to_open, PATH_MAX, "/proc/%d/cwd", pid);  
    read_link(path_to_open, "log.txt", "a", buffer);
}

int print_environ(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE];
    
    snprintf(path_to_open, PATH_MAX, "/proc/%d/environ", pid);  
    read_file(path_to_open, "log.txt", "a", buffer);
}

void print_exe(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE] = {'\0'};
    char *name_file = "\n\nexe:\n\0";
    
    snprintf(path_to_open, PATH_MAX, "/proc/%d/exe", pid);  
    read_link(path_to_open, "log.txt", "a", buffer);
}

void print_fd(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE];
    char str[PATH_MAX];
    char path[BUFFER_SIZE] = {'\0'};
    char *name_file = "\n\nfd:\n\0";
    FILE *file, *file_log;
    struct dirent *dir_p;
    DIR *dp;

    snprintf(path_to_open, PATH_MAX, "/proc/%d/fd/", pid);  
    file = fopen(path_to_open, "r");
    file_log = fopen("log.txt", "a");

    fwrite(path_to_open, strlen(path_to_open), 1, file_log);
    fwrite(":\n", 1, 2, file_log);
    dp = opendir(path_to_open);
    while ((dir_p = readdir(dp)) != NULL)
    {
        if ((strcmp(dir_p->d_name, ".") != 0) &&
            (strcmp(dir_p->d_name, "..") != 0))
            {
                sprintf(path, "%s%s", path_to_open, dir_p->d_name);
                readlink(path, buffer, BUFFER_SIZE);
                fwrite(dir_p->d_name, strlen(dir_p->d_name), 1, file_log);
                fwrite(" -> ", 4, 1, file_log);
                fwrite(buffer, strlen(buffer), 1, file_log);
                fwrite("\n", 1, 1, file_log);
                printf("%s -> %s\n", dir_p->d_name, buffer);
            }
    }
    closedir(dp);
    fwrite("\n\n", 1, 2, file_log);
    fclose(file);
    fclose(file_log);
}

void print_root(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE] = {'\0'};

    snprintf(path_to_open, PATH_MAX, "/proc/%d/root", pid);  
    read_link(path_to_open, "log.txt", "a", buffer);
}

void print_stat(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE];
    snprintf(path_to_open, PATH_MAX, "/proc/%d/stat", pid);
    FILE *file = fopen(path_to_open, "r"),
         *file_log = fopen("log.txt", "a");
    fwrite(path_to_open, strlen(path_to_open), 1, file_log);
    fwrite(":\n", 1, 2, file_log);

    fread(buffer, 1, BUFFER_SIZE, file);
    char *token = strtok(buffer, " ");

    //printf("\n\nstat:\n");
    for (int i = 0; token != NULL; i++)
    {
        fwrite(stat_info[i], strlen(stat_info[i]), 1, file_log);
        fwrite(token, strlen(token), 1, file_log);
        fwrite("\n", 1, 1, file_log);
        //printf("%s %s\n", stat_info[i], token);
        token = strtok(NULL, " ");
    }
    fwrite("\n\n", 1, 2, file_log);

    fclose(file);
    fclose(file_log);
}

void print_maps(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE];

    snprintf(path_to_open, PATH_MAX, "/proc/%d/maps", pid);
    read_file(path_to_open, "log.txt", "a", buffer);
}

void print_io(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE];
    
    snprintf(path_to_open, PATH_MAX, "/proc/%d/io", pid);  
    read_file(path_to_open, "log.txt", "a", buffer);
}

void print_comm(const int pid)
{
    char path_to_open[PATH_MAX];
    char buffer[BUFFER_SIZE];

    snprintf(path_to_open, PATH_MAX, "/proc/%d/comm", pid);  
    read_file(path_to_open, "log.txt", "a", buffer);
}

void print_task(const int pid)
{
    char path_to_open[PATH_MAX];

    snprintf(path_to_open, PATH_MAX, "/proc/%d/task", pid);  
    //printf("\n\ntask:\n");

    FILE *file = fopen(path_to_open, "r"),
         *file_log = fopen("log.txt", "a");
    fwrite(path_to_open, strlen(path_to_open), 1, file_log);
    fwrite(":\n", 1, 2, file_log);

    do_path(path_to_open, func_tree, 0, file, file_log);

    fclose(file);
    fclose(file_log);
}

int do_path(const char *path_name, int (*func)(const char *, const struct stat *, int, int), int depth,
            FILE *file, FILE *file_log)
{
    struct stat statbuf; struct dirent *dirp;
    DIR *dp; 
    int error = 0;

    if (strcmp(path_name, ".") == 0 || strcmp(path_name, "..") == 0){
        return 0;
    }

    if (lstat(path_name, &statbuf) == -1){
        /* ошибка вызова lstat */
        return (func_tree(path_name, &statbuf, FTW_NS, depth,
                        file, file_log));
    }

    if (S_ISDIR(statbuf.st_mode) == 0){
        /* не директория (остальные типы файлов)*/
        return (func_tree(path_name, &statbuf, FTW_F, depth,
                        file, file_log));
    }

    func_tree(path_name, &statbuf, FTW_D, depth,
                        file, file_log);

    if ((dp = opendir(path_name)) == NULL){
        /* каталог недоступен*/
        return (func_tree(path_name, &statbuf, FTW_DNR, depth,
                        file, file_log));
    }
    
    chdir(path_name);

    while ((dirp = readdir(dp)) != NULL && error == 0){
        error = do_path(dirp->d_name, func, depth + 1,
                        file, file_log);
    }

    chdir("..");

    if (closedir(dp) < 0){
        fprintf(stderr, "Невозможно закрыть каталог(%s): %d.\n", path_name, -1);
        return -1;
    }

    return error;
}

int func_tree(const char *path_name, const struct stat *stat_ptr, int type,
              int depth, FILE *file, FILE *file_log)
{
    for (int i = 0; i < depth; i++){
        fwrite("│    ", 6, 1, file_log);
        //printf("│  ");
    }

    switch(type)
    {
        case FTW_F:
            switch (stat_ptr->st_mode & S_IFMT)
            {
                case S_IFDIR:
                    fprintf(stderr, "Каталоги должны иметь тип FTW_D.\n");
                    return -1;
                default:
                {
                    //printf("├── %s, %lu\n", path_name, stat_ptr->st_ino);
                    fwrite("|--", 3, 1, file_log);
                    fwrite(path_name, strlen(path_name), 1, file_log);
                    fwrite(", ", 2, 1, file_log);
                    //fwrite(stat_ptr->st_ino, strlen(stat_ptr->st_ino), 1, file_log);
                    fwrite("\n", 1, 1, file_log);
                    break;
                }
            }
            break;
        case FTW_D:
        {
            //printf("├── %s, %lu\n", path_name, stat_ptr->st_ino);
            fwrite("|--", 3, 1, file_log);
            fwrite(path_name, strlen(path_name), 1, file_log);
            fwrite(", ", 2, 1, file_log);
            fwrite("\n", 1, 1, file_log);
            break;
        }
        case FTW_DNR:
            fprintf(stderr, "Закрыт путь к каталогу %s.\n", path_name);
            return -1;
        case FTW_NS:
            fprintf(stderr, "Ошибка вызова для функции stat для %s.\n", path_name);
            return -1;
        default:
            fprintf(stderr, "Неизвестный тип %d для файла %s.\n", type, path_name);
            return -1;   
    }
    return 0;
}

