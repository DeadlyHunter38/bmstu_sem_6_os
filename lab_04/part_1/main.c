#include <stdio.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 0x100

int get_pid(int argc, char *argv[]);
int print_environ(const int pid);
int print_cmdline(const int pid);
int print_cwd(const int pid);

void read_file(FILE *file, FILE *file_log, char *buffer);

int main(int argc, char *argv[])
{   
    int pid = get_pid(argc, argv);

    if (pid != -1){
        print_cmdline(pid);
        print_environ(pid);
        print_cwd(pid);
    }
    return 0;
}

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
    char opened_path[PATH_MAX];
    char buffer[BUFFER_SIZE];
    char name_file[] = "cmdline:\n\0";
    FILE *file, *file_log;

    snprintf(opened_path, PATH_MAX, "/proc/%d/cmdline", pid);
    file = fopen(opened_path, "r");
    file_log = fopen("log.txt", "w");

    fwrite(&name_file, strlen(name_file), 1, file_log);
    read_file(file, file_log, buffer);

    fclose(file);
    fclose(file_log);
}

int print_cwd(const int pid)
{
    char opened_path[PATH_MAX];
    char buffer[BUFFER_SIZE] = {'\0'};
    char *name_file = "\n\ncwd:\n\0";
    FILE *file, *file_log;

    snprintf(opened_path, PATH_MAX, "/proc/%d/cwd", pid);  
    file = fopen(opened_path, "r");
    file_log = fopen("log.txt", "a");

    fwrite(&name_file, strlen(name_file), 1, file_log);
    
    read_file(file, file_log, buffer);

    fclose(file);
    fclose(file_log);
}

int print_environ(const int pid)
{
    char opened_path[PATH_MAX];
    char buffer[BUFFER_SIZE];
    char *name_file = "\n\nenviron:\n\0";
    FILE *file, *file_log;

    snprintf(opened_path, PATH_MAX, "/proc/%d/environ", pid);  
    file = fopen(opened_path, "r");
    file_log = fopen("log.txt", "a");

    fwrite(&name_file, strlen(name_file), 1, file_log);
    
    read_file(file, file_log, buffer);

    fclose(file);
    fclose(file_log);
}

void read_file(FILE *file, FILE *file_log, char *buffer)
{
    
    int len;
    while ((len = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        for (int i = 0; i < len; i++){
            if (buffer[i] == 0){
                buffer[i] = '\n';
            }
        }

        buffer[len - 1]= '\n';
        //printf("%s", buffer);
        fwrite(&buffer, BUFFER_SIZE, 1, file_log);
    }
}