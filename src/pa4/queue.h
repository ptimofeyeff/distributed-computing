#ifndef LAB1_QUEUE_H
#define LAB1_QUEUE_H

#include "ipc.h"
#include "criticalSection.h"
#include <stdbool.h>

#define MAX_QUEUE_SIZE 2048

typedef struct {
    Request requests[MAX_QUEUE_SIZE];
    int length;
} Queue;

bool enqueue(Request request);

Request dequeue();

Request peek();

int compare(Request, Request);

#endif //LAB1_QUEUE_H
