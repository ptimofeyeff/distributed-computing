#ifndef LAB1_CRITICALSECTION_H
#define LAB1_CRITICALSECTION_H

#include "ipc.h"
#include "branch.h"
#include "workers.h"

typedef struct {
    timestamp_t time;
    local_id procId;
} Request;

void syncReceiveCs(BranchData *branchData);

Request sendCsRequest(BranchData *branchData);

void receiveAllRepliesHandler(BranchData *branchData, Request, Workers);

void receiveCsRequestAndSendReply(BranchData *branchData, Message, Request);

void receiveCsRelease(BranchData *branchData, Message release);

#endif //LAB1_CRITICALSECTION_H
