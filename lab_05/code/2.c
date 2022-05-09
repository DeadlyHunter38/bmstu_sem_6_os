#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>

#define FILE_NAME "alphabet.txt"
pthread_mutex_t lock;

void *run(void *arg);

int main()
{
    char c;
    pthread_t tid1, tid2;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        perror("failed mitex init");
        return -1;
    }

    int fd1 = open(FILE_NAME, O_RDONLY);
    int fd2 = open(FILE_NAME, O_RDONLY);

    pthread_create(&tid1, NULL, run, &fd1);
    pthread_create(&tid2, NULL, run, &fd2);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_mutex_destroy(&lock);

    close(fd1);
    close(fd2);

    return 0;
}

void *run(void *arg)
{
    int fd = *(int*)arg;
    int flag = 1; 
    char c;

    pthread_mutex_lock(&lock);
    while (flag == 1){
        flag = read(fd, &c, 1);
        if (flag == 1){
            write(1, &c, 1);
        }
    }
    pthread_mutex_unlock(&lock);

    return 0;
}