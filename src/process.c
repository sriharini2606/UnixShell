#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#include "../include/process.h"

void showProcesses()
{
    FILE *file;
    char line[300];

    file = popen("ps -e", "r");

    if (file == NULL)
    {
        printf("Unable to read process information.\n");
        return;
    }

    printf("\n========== PROCESS INFORMATION ==========\n");

    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("%s", line);
    }

    printf("=========================================\n");

    pclose(file);
}
