#include <fcntl.h>
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syslog.h>

#include "../includes/daemon.h"

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int lockfile(int);
void reread(void);

void daemonize(const char *cmd)
{
    int fd_0, fd_1, fd_2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
        
    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0){
        fprintf(stderr, "%s: невозможно получить максимальный номер дескриптора.", cmd);
    }

    if ((pid = fork()) < 0){
        fprintf(stderr, "%s: ошибка вызова функции fork.", cmd);
    }
    else if (pid != 0){
        fprintf(stderr, "%s: pid != 0.\n", cmd);
        exit(0);
    }
    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0){
        fprintf(stderr, "%s: невозможно игнорировать сигнал SIGHUP.", cmd);
        exit(1);
    }

    if (chdir("/") < 0){
        fprintf(stderr, "%s: невозможно сделать текущим рабочим каталогом /", cmd);
        exit(1);
    }

    if (rl.rlim_max == RLIM_INFINITY){
        rl.rlim_max = 1024;
    }

    for (int i = 0; i < rl.rlim_max; i++){
        close(i);
    }

    fd_0 = open("/dev/null", O_RDWR);
    fd_1 = dup(0);
    fd_2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd_0 != 0 || fd_1 != 1 || fd_2 != 2){
        syslog(LOG_ERR, "Ошибочные файловые дескрипторы %d %d %d", fd_0, fd_1, fd_2);
        exit(1);
    }
}

int already_running(void)
{
    int fd;
    char buffer[16];

    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);

    if (fd < 0){
        syslog(LOG_ERR, "Невозможно открыть %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }

    if (lockfile(fd) < 0){
        if (errno == EACCES || errno == EAGAIN){
            close(fd);
            return 1;
        }

        syslog(LOG_ERR, "невозможно установить блокировку на %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }

    ftruncate(fd, 0);
    sprintf(buffer, "%ld", (long)getpid());
    write(fd, buffer, strlen(buffer) + 1);
    return 0;
}

int lockfile(int fd)
{
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return fcntl(fd, F_SETLK, &fl);
}

void sighup(int signo){
    printf("sighup.\n");
    syslog(LOG_INFO, "Чтение конфигурационного файла.");
}

void sigterm(int signo){
    printf("sigterm.\n");
    syslog(LOG_INFO, "Получен сигнал SIGTERM; выход.");
}

void *thr_fn(void *arg)
{
    int error, signo;
    for (;;)
    {
        error = sigwait(&mask, &signo);
        if (error != 0){
            syslog(LOG_ERR, "ошибка вызова функции sigwait");
            exit(1);
        }

        switch (signo)
        {
            case SIGHUP:
                syslog(LOG_INFO, "Чтение конфигурационного файла.");
                reread();
                break;
            case SIGTERM:
                syslog(LOG_INFO, "Получен сигнал SIGTERM; выход");
                exit(0);
            default:
                syslog(LOG_INFO, "получен непредвиденный сигнал %d\n", signo);
        }
    }

    return 0;
}

void reread(void){
    syslog(LOG_INFO, "reread function");
}