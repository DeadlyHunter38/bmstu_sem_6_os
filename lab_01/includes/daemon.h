#ifndef _DAEMON_H_
#define _DAEMON_H_

#include <syslog.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void daemonize(const char *cmd);
int already_running(void);

void *thr_fn(void *arg);

static sigset_t mask;

#endif