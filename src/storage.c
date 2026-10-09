#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#include "../include/storage.h"

void showStorage()
{
    FILE *file;
    char line[200];

    file = popen("df -h", "r");

    if (file == NULL)
    {
        printf("Unable to read storage information.\n");
        return;
    }

    printf("\n========== STORAGE INFORMATION ==========\n");

    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("%s", line);
    }

    printf("=========================================\n");

    pclose(file);
}
