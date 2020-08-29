#include "workers.h"
#include "stdio.h"

Workers workers;

void initWorkers(int workerCount) {
    workers.length = workerCount - 1;
    for (int i = 0; i < workers.length; ++i) {
        workers.procId[i] = i + 1;
    }
}

void deleteWorker(local_id id) {
    for (int i = 0; i <workers.length; ++i) {
        if (workers.procId[i] == id) {
            for (int j = i; j <workers.length - 1; ++j) {
                workers.procId[j] = workers.procId[j + 1];
            }
            break;
        }
    }
    workers.length--;
}

Workers getWorkers() {
    return workers;
}

void printWorkers() {
    printf("[ ");
    for (int i = 0; i <workers.length; ++i) {
        printf("%d, ", workers.procId[i]);
    }
    printf(" ]\n");
}
