#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include "./includes/daemon.h"

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
        ttime = time (NULL);
        syslog(LOG_INFO, "Время: %s\n", ctime (&ttime));
    }
    return 0;
}