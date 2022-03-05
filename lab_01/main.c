#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#include "./includes/daemon.h"

static sigset_t mask;
void *thr_fn(void *arg);

int main(int argc, char *argv[])
{
    char *cmd;
    struct sigaction sa;
    pthread_t tid;
    int error;

    if ((cmd = strrchr(argv[0], '/')) == NULL){
        cmd =  argv[0];
    }
    else{
        cmd++;
    }

    daemonize(cmd);

    if (already_running()){
        syslog(LOG_ERR, "Демон уже запущен.");
        exit(1);
    }

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0){
        syslog(LOG_ERR, "невозможно восстановить действие SIG_DFL для SIGHUP.\n");
        exit(1);
    }

    sigfillset(&mask);
    if ((error = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0){
        syslog(LOG_ERR, "ошибка выполнения операции SIG_BLOCK.\n");
        exit(1);
    }

    error = pthread_create(&tid, NULL, thr_fn, 0);
    if (error != 0){
        syslog(LOG_ERR, "Невозможно создать поток.\n");
        exit(1);
    }

    syslog(LOG_INFO, "Демон выполняется.");

    long int ttime;
    while(true){
        sleep(5);
        ttime = time (NULL);
        syslog(LOG_INFO, "Время: %s\n", ctime (&ttime));
    }
    return 0;
}

void *thr_fn(void *arg)
{
    int error, signo;
    for (;;)
    {
        error = sigwait(&mask, &signo);
        if (error != 0){
            syslog(LOG_ERR, "Ошибка вызова функции sigwait.");
            exit(1);
        }

        switch (signo)
        {
            case SIGHUP:
                syslog(LOG_INFO, "Чтение конфигурационного файла.");
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