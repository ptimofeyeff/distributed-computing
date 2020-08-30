#ifndef LAB1_WORKERS_H
#define LAB1_WORKERS_H

#include "branch.h"


typedef struct {
    local_id procId[MAX_PROCESS_ID];
    int length;
} Workers;

Workers getWorkers();

void initWorkers(int);
void deleteWorker(local_id id);
void printWorkers();


#endif //LAB1_WORKERS_H
