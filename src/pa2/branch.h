#ifndef LAB1_BRANCH_H
#define LAB1_BRANCH_H

#include <stdlib.h>
#include <sys/wait.h>

#include "ipc.h"
#include "logs.h"

#define DESC_COUNT 2
#define READ_DESC 0
#define WRITE_DESC 1
#define MAX_PROC 11

typedef struct {
    int descriptors[MAX_PROC][MAX_PROC][DESC_COUNT];
} BranchDescriptors;

typedef struct {
    BranchDescriptors *descriptors;
    int branchCount;
    local_id id;
    balance_t balance;
} BranchData;

void createBranch(BranchData *, const balance_t[]);

void run(BranchData *);

void waitChild(int count);



#endif //LAB1_BRANCH_H
