#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

#include "../include/command.h"
#include "../include/vector.h"
#include "../include/parser.h"

/* Store the last command status */
static int lastStatus = 0;


/*
 * Run one command.
 *
 * This function also handles:
 *
 * <   input redirection
 * >   output redirection
 * >>  output append redirection
 */
void runSingleCommand(char *command)
{
    StringVector args;

    initVector(&args);

    char *token = strtok(command, " \t");

    while (token != NULL)
    {
        /*
         * Input redirection
         */
        if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " \t");

            if (token == NULL)
            {
                printf("Error: input file not specified.\n");
                freeVector(&args);
                exit(1);
            }

            int fd = open(token, O_RDONLY);

            if (fd < 0)
            {
                perror("Input redirection");
                freeVector(&args);
                exit(1);
            }

            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        /*
         * Output redirection
         */
        else if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " \t");

            if (token == NULL)
            {
                printf("Error: output file not specified.\n");
                freeVector(&args);
                exit(1);
            }

            int fd = open(
                token,
                O_WRONLY | O_CREAT | O_TRUNC,
                0644
            );

            if (fd < 0)
            {
                perror("Output redirection");
                freeVector(&args);
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        /*
         * Append redirection
         */
        else if (strcmp(token, ">>") == 0)
        {
            token = strtok(NULL, " \t");

            if (token == NULL)
            {
                printf("Error: output file not specified.\n");
                freeVector(&args);
                exit(1);
            }

            int fd = open(
                token,
                O_WRONLY | O_CREAT | O_APPEND,
                0644
            );

            if (fd < 0)
            {
                perror("Append redirection");
                freeVector(&args);
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        /*
         * Normal command argument
         */
        else
        {
            addToVector(&args, token);
        }

        token = strtok(NULL, " \t");
    }

    addToVector(&args, NULL);

    /*
     * Restore default signal behavior
     * for the child process.
     */
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    /*
     * Execute the command.
     */
    if (args.size <= 1 || args.items[0] == NULL)
    {
        freeVector(&args);
        exit(0);
    }

    execvp(args.items[0], args.items);

    printf("Command not found: %s\n", args.items[0]);

    freeVector(&args);

    exit(1);
}


/* Execute multiple commands connected using pipes */
void executePipeline(StringVector *commands)
{
    int numberOfCommands = commands->size - 1;

    int previousPipeRead = -1;

    pid_t children[numberOfCommands];

    for (int i = 0; i < numberOfCommands; i++)
    {
        int pipefd[2];

        /*
         * Create a pipe except after the last command.
         */
        if (i < numberOfCommands - 1)
        {
            if (pipe(pipefd) == -1)
            {
                printf("Pipe creation failed.\n");
                return;
            }
        }

        children[i] = fork();

        if (children[i] < 0)
        {
            printf("Fork failed.\n");
            return;
        }

        if (children[i] == 0)
        {
            /*
             * Restore default signal behavior
             * for child processes.
             */
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            /*
             * If this is not the first command,
             * take input from the previous pipe.
             */
            if (previousPipeRead != -1)
            {
                dup2(previousPipeRead, STDIN_FILENO);
                close(previousPipeRead);
            }

            /*
             * If this is not the last command,
             * send output into the next pipe.
             */
            if (i < numberOfCommands - 1)
            {
                dup2(pipefd[1], STDOUT_FILENO);

                close(pipefd[0]);
                close(pipefd[1]);
            }

            /*
             * Run command.
             *
             * Redirection is handled inside
             * runSingleCommand().
             */
            runSingleCommand(commands->items[i]);
        }

        /*
         * Parent closes the previous pipe
         * because it is no longer needed.
         */
        if (previousPipeRead != -1)
        {
            close(previousPipeRead);
        }

        /*
         * Parent keeps the read end of the
         * current pipe for the next command.
         */
        if (i < numberOfCommands - 1)
        {
            close(pipefd[1]);
            previousPipeRead = pipefd[0];
        }
    }

    /*
     * Parent no longer needs the last read end.
     */
    if (previousPipeRead != -1)
    {
        close(previousPipeRead);
    }

    /*
     * Wait for all child processes.
     */
    for (int i = 0; i < numberOfCommands; i++)
    {
        waitpid(children[i], NULL, 0);
    }
}


/* Execute a command entered by the user */
void executeCommand(char *input)
{
    StringVector commands;

    initVector(&commands);

    parseCommand(input, &commands);

    if (commands.size == 0)
    {
        freeVector(&commands);
        return;
    }

    /*
     * commands.size includes the final NULL.
     *
     * Example:
     *
     * ls | grep .c | wc -l
     *
     * commands:
     * [0] ls
     * [1] grep .c
     * [2] wc -l
     * [3] NULL
     *
     * Therefore size = 4.
     */
    int numberOfCommands = commands.size - 1;

    /*
     * Multiple commands connected by pipes.
     */
    if (numberOfCommands > 1)
    {
        executePipeline(&commands);

        freeVector(&commands);
        return;
    }

    /*
     * Normal single command.
     */
    char *command = commands.items[0];

    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Fork failed.\n");

        freeVector(&commands);

        return;
    }

    if (pid == 0)
    {
        /*
         * runSingleCommand handles:
         *
         * - argument parsing
         * - input redirection
         * - output redirection
         * - append redirection
         * - execvp()
         */
        runSingleCommand(command);
    }
    else
    {
        int status;

        /*
         * WUNTRACED allows the parent to detect
         * when the child is stopped by Ctrl+Z.
         */
        waitpid(pid, &status, WUNTRACED);

        if (WIFSTOPPED(status))
        {
            printf("\nProcess suspended by Ctrl+Z\n");
        }
        else if (WIFEXITED(status))
        {
            lastStatus = WEXITSTATUS(status);

            printf("Command exited with status: %d\n", lastStatus);
        }
    }

    freeVector(&commands);
}


/* Return the last command status */
int getLastStatus()
{
    return lastStatus;
}
