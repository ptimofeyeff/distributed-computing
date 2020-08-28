#ifndef LAB1_CRITICALSECTION_H
#define LAB1_CRITICALSECTION_H

#include "ipc.h"
#include "branch.h"

typedef struct {
    timestamp_t time;
    local_id procId;
} Request;

void syncReceiveCs(BranchData *branchData);
Request sendAndSaveCsRequest(BranchData *branchData);
void syncReceiveCsReplies(BranchData *branchData, Request);
int checkEnterCondition(BranchData *branchData, Request currentRequest);
void receiveCsRequestAndSendReply(BranchData *branchData, Message);
void receiveCsRelease(BranchData *branchData, Message release);
#endif //LAB1_CRITICALSECTION_H
