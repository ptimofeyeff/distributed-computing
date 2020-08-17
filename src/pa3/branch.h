#ifndef LAB1_BRANCH_H
#define LAB1_BRANCH_H

#include "ipc.h"
#include "banking.h"
#include "descriptors.h"

timestamp_t logicTime;

typedef struct {
    TopologyDescriptors *descriptors;
    int branchCount;
    local_id id;
    balance_t balance;
    timestamp_t logicTime;
} BranchData;

void createBranch(TopologyDescriptors *, const balance_t[], int);

void run(BranchData *);

void waitChild(int count);


#endif //LAB1_BRANCH_H
