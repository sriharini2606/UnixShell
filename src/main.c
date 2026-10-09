#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "../include/command.h"
#include "../include/memory.h"
#include "../include/storage.h"
#include "../include/process.h"
#include "../include/report.h"
#include "../include/signal.h"

int main()
{
    setupSignals();

    char *input = NULL;
    size_t inputSize = 0;

    printf("=====================================\n");
    printf("        Welcome to UnixShell\n");
    printf("=====================================\n");

    while (1)
    {
        printf("UnixShell> ");
        fflush(stdout);

        errno = 0;

        if (getline(&input, &inputSize, stdin) == -1)
        {
            /*
             * If a signal interrupted getline(),
             * continue running the shell.
             */
            if (errno == EINTR)
            {
                clearerr(stdin);
                continue;
            }

            break;
        }

        input[strcspn(input, "\n")] = '\0';

        /* Exit UnixShell */
        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting UnixShell...\n");
            break;
        }

        /* Memory information */
        if (strcmp(input, "memory") == 0)
        {
            showMemory();
            continue;
        }

        /* Virtual memory statistics */
        if (strcmp(input, "memstat") == 0)
        {
            showMemStat();
            continue;
        }

        /* Storage information */
        if (strcmp(input, "storage") == 0)
        {
            showStorage();
            continue;
        }

        /* Process information */
        if (strcmp(input, "process") == 0)
        {
            showProcesses();
            continue;
        }

        /* System report */
        if (strcmp(input, "report") == 0)
        {
            generateReport();
            continue;
        }

        /* Help */
        if (strcmp(input, "help") == 0)
        {
            printf("\n========== UNIXSHELL COMMANDS ==========\n");
            printf("pwd      - Show current directory\n");
            printf("ls       - List files and directories\n");
            printf("whoami   - Show current user\n");
            printf("date     - Show current date and time\n");
            printf("memory   - Show memory information\n");
            printf("memstat  - Show virtual memory statistics\n");
            printf("storage  - Show storage information\n");
            printf("process  - Show running processes\n");
            printf("report   - Generate system report\n");
            printf("help     - Show available commands\n");
            printf("clear    - Clear the terminal screen\n");
            printf("exit     - Exit UnixShell\n");
            printf("========================================\n");

            continue;
        }

        /* Clear terminal */
        if (strcmp(input, "clear") == 0)
        {
            system("clear");
            continue;
        }

        /* Ignore empty input */
        if (strlen(input) == 0)
        {
            continue;
        }

        /* Execute normal UNIX command */
        executeCommand(input);
    }

    free(input);

    return 0;
}
