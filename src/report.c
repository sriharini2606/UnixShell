#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/statvfs.h>

#define KB_TO_GIB 1048576.0

static int getMemoryInfo(
    unsigned long long *total,
    unsigned long long *available)
{
    FILE *file = fopen("/proc/meminfo", "r");

    if (file == NULL)
    {
        perror("Unable to read memory information");
        return 0;
    }

    char line[256];
    int foundTotal = 0;
    int foundAvailable = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "MemTotal: %llu kB", total) == 1)
            foundTotal = 1;

        if (sscanf(line, "MemAvailable: %llu kB", available) == 1)
            foundAvailable = 1;
    }

    fclose(file);

    return foundTotal && foundAvailable && *total > 0;
}

static int countProcesses(void)
{
    DIR *directory = opendir("/proc");

    if (directory == NULL)
        return -1;

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(directory)) != NULL)
    {
        if (isdigit((unsigned char)entry->d_name[0]))
        {
            int isProcess = 1;

            for (int i = 0; entry->d_name[i] != '\0'; i++)
            {
                if (!isdigit((unsigned char)entry->d_name[i]))
                {
                    isProcess = 0;
                    break;
                }
            }

            if (isProcess)
                count++;
        }
    }

    closedir(directory);
    return count;
}

static void printMemoryReport(void)
{
    unsigned long long total = 0;
    unsigned long long available = 0;

    printf("\n----- MEMORY ANALYSIS -----\n");

    if (!getMemoryInfo(&total, &available))
    {
        printf("Memory information is unavailable.\n");
        return;
    }

    if (available > total)
        available = total;

    unsigned long long used = total - available;
    double usage = 100.0 * (double)used / (double)total;

    printf("Total memory     : %.2f GiB\n",
           total / KB_TO_GIB);

    printf("Used memory      : %.2f GiB\n",
           used / KB_TO_GIB);

    printf("Available memory : %.2f GiB\n",
           available / KB_TO_GIB);

    printf("Memory usage     : %.1f%%\n", usage);

    if (usage >= 80.0)
    {
        printf("Memory status    : HIGH\n");
        printf("Suggestion       : Investigate memory-consuming "
               "processes and close unnecessary applications.\n");
    }
    else if (usage >= 60.0)
    {
        printf("Memory status    : MODERATE\n");
        printf("Suggestion       : Monitor memory usage and "
               "check for increasing consumption.\n");
    }
    else
    {
        printf("Memory status    : LOW\n");
        printf("Suggestion       : Memory usage is currently "
               "low. Continue monitoring.\n");
    }
}

static int printStorageReport(void)
{
    struct statvfs info;

    printf("\n----- STORAGE ANALYSIS -----\n");
    printf("Filesystem checked: /\n");

    if (statvfs("/", &info) != 0)
    {
        perror("Unable to read storage information");
        return -1;
    }

    if (info.f_blocks == 0 || info.f_frsize == 0)
    {
        printf("Storage information is unavailable.\n");
        return -1;
    }

    double total = (double)info.f_blocks * info.f_frsize;
    double freeSpace = (double)info.f_bavail * info.f_frsize;
    double used = total -
                  (double)info.f_bfree * info.f_frsize;

    if (used < 0)
        used = 0;

    double usage = 100.0 * used / total;

    if (usage > 100.0)
        usage = 100.0;

    printf("Total space      : %.2f GiB\n",
           total / 1073741824.0);

    printf("Used space       : %.2f GiB\n",
           used / 1073741824.0);

    printf("Available space  : %.2f GiB\n",
           freeSpace / 1073741824.0);

    printf("Storage usage    : %.1f%%\n", usage);

    if (usage >= 90.0)
    {
        printf("Storage status   : HIGH\n");
        printf("Suggestion       : Check large files and "
               "remove unnecessary files safely.\n");
    }
    else if (usage >= 75.0)
    {
        printf("Storage status   : MODERATE\n");
        printf("Suggestion       : Monitor disk space and "
               "review unnecessary files.\n");
    }
    else
    {
        printf("Storage status   : LOW\n");
        printf("Suggestion       : Sufficient free space is "
               "currently available. Continue monitoring.\n");
    }

    return usage >= 90.0 ? 2 : usage >= 75.0 ? 1 : 0;
}

static int getMemoryLevel(void)
{
    unsigned long long total = 0;
    unsigned long long available = 0;

    if (!getMemoryInfo(&total, &available))
        return -1;

    if (available > total)
        available = total;

    double usage = 100.0 * (double)(total - available) /
                   (double)total;

    if (usage >= 80.0)
        return 2;

    if (usage >= 60.0)
        return 1;

    return 0;
}

void generateReport(void)
{
    printf("\n=========================================\n");
    printf("       UNIX SYSTEM ANALYSIS REPORT\n");
    printf("=========================================\n");

    int memoryLevel = getMemoryLevel();

    printMemoryReport();

    int storageLevel = printStorageReport();

    printf("\n----- PROCESS ANALYSIS -----\n");

    int processes = countProcesses();

    if (processes >= 0)
    {
        printf("Running processes: %d\n", processes);
        printf("Suggestion        : If the system becomes slow, "
               "inspect individual processes for high CPU "
               "or memory usage.\n");
    }
    else
    {
        printf("Process information is unavailable.\n");
    }

    printf("\n----- FINAL RECOMMENDATION -----\n");

    if (memoryLevel == 2 || storageLevel == 2)
    {
        printf("System status: ATTENTION REQUIRED\n");
        printf("Recommendation: Investigate the resource "
               "reported as HIGH above.\n");
    }
    else if (memoryLevel == 1 || storageLevel == 1)
    {
        printf("System status: MONITOR RESOURCES\n");
        printf("Recommendation: Monitor memory or storage "
               "usage and take action if it increases.\n");
    }
    else if (memoryLevel == 0 && storageLevel == 0)
    {
        printf("System status: RESOURCES APPEAR SUFFICIENT\n");
        printf("Recommendation: No immediate memory or "
               "storage action is indicated by these "
               "measurements.\n");
    }
    else
    {
        printf("System status: PARTIAL ANALYSIS\n");
        printf("Recommendation: Check the unavailable "
               "resource information before drawing "
               "a complete conclusion.\n");
    }

    printf("Note: Process count alone does not indicate "
           "system health.\n");

    printf("=========================================\n");
    printf("           REPORT COMPLETE\n");
    printf("=========================================\n");
}
