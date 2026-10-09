#include <stdio.h>
#include <stdlib.h>

#include "../include/vector.h"

void initVector(StringVector *vector)
{
    vector->size = 0;
    vector->capacity = 4;

    vector->items = malloc(vector->capacity * sizeof(char *));

    if (vector->items == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }
}

void addToVector(StringVector *vector, char *item)
{
    if (vector->size == vector->capacity)
    {
        vector->capacity = vector->capacity * 2;

        vector->items = realloc(
            vector->items,
            vector->capacity * sizeof(char *)
        );

        if (vector->items == NULL)
        {
            printf("Memory reallocation failed.\n");
            exit(1);
        }
    }

    vector->items[vector->size] = item;
    vector->size++;
}

void freeVector(StringVector *vector)
{
    free(vector->items);

    vector->items = NULL;
    vector->size = 0;
    vector->capacity = 0;
}
