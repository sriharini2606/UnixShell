#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
    char **items;
    int size;
    int capacity;
} StringVector;

void initVector(StringVector *vector);
void addToVector(StringVector *vector, char *item);
void freeVector(StringVector *vector);

#endif
