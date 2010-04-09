/*
 *  NOTE -- must setup signals per thread
 */
#include    "stdlib.h"
#include    "signal.h"
#include    "stdio.h"
#include    "string.h"
#include    "setjmp.h"
#include    "unistd.h"

jmp_buf jbuf;
static void catchSignal(int signo, siginfo_t *info, void *arg)
{
    printf("SIGNO %d\n", signo);
    longjmp(jbuf, 1);
}


int main()
{
    struct sigaction    act;
    char *p;

    memset(&act, 0, sizeof(act));
    act.sa_sigaction = catchSignal;
    act.sa_flags = 0;
   
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGQUIT);
    sigaddset(&act.sa_mask, SIGCHLD);
    sigaddset(&act.sa_mask, SIGALRM);
    sigaddset(&act.sa_mask, SIGPIPE);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaddset(&act.sa_mask, SIGUSR1);
    sigaddset(&act.sa_mask, SIGUSR2);

    /*
     *  Catch thse signals
     */
    sigaction(SIGSEGV, &act, 0);
    sigaction(SIGBUS, &act, 0);

    if (setjmp(jbuf) == 0) {
        *p = 0;
    } else {
        printf("AFTER\n");
    }
}
