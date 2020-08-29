#ifndef LAB1_BRANCH_H
#define LAB1_BRANCH_H

#include "ipc.h"
#include "descriptors.h"
#include "stdbool.h"

typedef struct {
    TopologyDescriptors *descriptors;
    int branchCount;
    local_id id;
    timestamp_t logicTime;
    local_id senderId;
    bool mutex;
} BranchData;

void createBranch(TopologyDescriptors *, int, bool);

void run(BranchData *);

void waitChild(int count);


#endif //LAB1_BRANCH_H
