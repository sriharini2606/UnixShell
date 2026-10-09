#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/signal.h"

void handleCtrlC(int signalNumber)
{
    if (signalNumber == SIGINT)
    {
        printf("\n");
        fflush(stdout);
    }
}

void handleCtrlZ(int signalNumber)
{
    if (signalNumber == SIGTSTP)
    {
        printf("\nCtrl+Z pressed - process suspended.\n");
        fflush(stdout);
    }
}

/* Handle finished child processes */
void handleSIGCHLD(int signalNumber)
{
    if (signalNumber == SIGCHLD)
    {
        int status;

        while (waitpid(-1, &status, WNOHANG) > 0)
        {
            /* Child process cleaned up */
        }
    }
}

void setupSignals(void)
{
    signal(SIGINT, handleCtrlC);
    signal(SIGTSTP, handleCtrlZ);
    signal(SIGCHLD, handleSIGCHLD);
}
