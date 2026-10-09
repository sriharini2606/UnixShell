#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/memory.h"

void showMemory()
{
    FILE *file;
    char line[200];

    file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        printf("Unable to read memory information.\n");
        return;
    }

    printf("\n========== MEMORY INFORMATION ==========\n");

    for (int i = 0; i < 5; i++)
    {
        if (fgets(line, sizeof(line), file) != NULL)
        {
            printf("%s", line);
        }
    }

    printf("========================================\n");

    fclose(file);
}


/* Show virtual memory information of UnixShell */
void showMemStat()
{
    FILE *file;
    char line[200];

    file = fopen("/proc/self/status", "r");

    if (file == NULL)
    {
        printf("Unable to read process memory information.\n");
        return;
    }

    printf("\n========== VIRTUAL MEMORY STAT ==========\n");

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strncmp(line, "VmSize:", 7) == 0 ||
            strncmp(line, "VmRSS:", 6) == 0 ||
            strncmp(line, "VmData:", 7) == 0 ||
            strncmp(line, "VmStk:", 6) == 0 ||
            strncmp(line, "VmExe:", 6) == 0 ||
            strncmp(line, "VmLib:", 6) == 0)
        {
            printf("%s", line);
        }
    }

    printf("=========================================\n");

    fclose(file);
}
