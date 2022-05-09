#include <stdio.h>
#include <string.h>

#include "../inc/read.h"
#include "../inc/const.h"

void read_file(char *path_to_open, char *file_out, char *file_mode, char *buffer)
{
    FILE *file, *file_log;
    int len = 0;

    file = fopen(path_to_open, "r");
    file_log = fopen(file_out, file_mode);
    //printf("%s\n", path_to_open);
    fwrite(path_to_open, strlen(path_to_open), 1, file_log);
    fwrite(":\n", 1, 2, file_log);
    
    while ((len = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        for (int i = 0; i < len; i++){
            if (buffer[i] == 0){
                buffer[i] = '\n';
            }
        }
        
        buffer[len - 1]= '\0';
        fwrite(buffer, strlen(buffer), 1, file_log);
        //printf("%s\n", buffer);
    }
    fwrite("\n\n", 1, 2, file_log);

    fclose(file_log);
    fclose(file);
}

void read_link(char *path_to_open, char *file_out, char *file_mode, char *buffer)
{
    FILE *file, *file_log;

    readlink(path_to_open, buffer, BUFFER_SIZE);

    file = fopen(path_to_open, "r");
    file_log = fopen(file_out, "a");

    fwrite(path_to_open, strlen(path_to_open), 1, file_log);
    fwrite(":\n", 1, 2, file_log);
    fwrite(buffer, strlen(buffer), 1, file_log);
    fwrite("\n\n", 1, 2, file_log);

    fclose(file);
    fclose(file_log);
}