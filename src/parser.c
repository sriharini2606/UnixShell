#include <stdio.h>
#include <string.h>

#include "../include/parser.h"

void parseCommand(char *input, StringVector *commands)
{
    char *token;

    token = strtok(input, "|");

    while (token != NULL)
    {
        addToVector(commands, token);

        token = strtok(NULL, "|");
    }

    addToVector(commands, NULL);
}
