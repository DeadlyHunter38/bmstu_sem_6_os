#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>

#define FILE_NAME "alphabet_2.txt"

void *run_1(void *arg);

int main()
{
    struct stat stat_buf_open, stat_buf_close;
    pthread_t tid1;
    FILE *fd1 = fopen(FILE_NAME, "w");
    FILE *fd2 = fopen(FILE_NAME, "w");

    lstat(FILE_NAME, &stat_buf_open);
    printf("open_file: inode=%lu, size=%lu\n", stat_buf_open.st_ino, stat_buf_open.st_size);

    pthread_create(&tid1, NULL, run_1, fd1);

    for (char c = 'b'; c <= 'z'; c += 2){
        fprintf(fd2, "%c", c);
    }

    pthread_join(tid1, NULL);

    fclose(fd1);
    fclose(fd2);

    lstat(FILE_NAME, &stat_buf_close);
    printf("close_file: inode=%lu, size=%lu\n", stat_buf_close.st_ino, stat_buf_close.st_size);

    return 0;
}

void *run_1(void *arg)
{
    FILE *fd1 = arg;
    for (char c = 'a'; c <= 'z'; c += 2){
        fprintf(fd1, "%c", c);
    }

    return 0;
}