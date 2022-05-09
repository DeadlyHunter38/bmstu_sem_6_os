#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 20
#define FILE_NAME "alphabet.txt"

void *run(void *arg);

int main()
{
    pthread_t tid1, tid2;
    char buffer_1[BUFFER_SIZE], buffer_2[BUFFER_SIZE];
    int fd = open(FILE_NAME, O_RDONLY);

    FILE *fd1 = fdopen(fd, "r");
    setvbuf(fd1, buffer_1, _IOFBF, BUFFER_SIZE);

    FILE *fd2 = fdopen(fd, "r");
    setvbuf(fd2, buffer_2, _IOFBF, BUFFER_SIZE);

    pthread_create(&tid1, NULL, run, fd1);
    pthread_create(&tid2, NULL, run, fd2);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    close(fd);

    return 0;
}

void *run(void *arg)
{
    FILE *fs = arg;
    int flag = 1;
    char c;

    while (flag == 1) {
        flag = fscanf(fs, "%c", &c);
        if (flag == 1)
            fprintf(stdout, "%c", c);
    }
    return NULL;
}