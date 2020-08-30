#include "criticalSection.h"
#include "pa2345.h"
#include "lamport.h"
#include "queue.h"
#include "message.h"
#include <stdio.h>
#include "ipcx.h"

int request_cs(const void * self) {
    BranchData *branchData = (BranchData *) self;

    Request thisRequest = sendCsRequest(branchData);

    if (getWorkers().length > 1) {
        Workers workers = getWorkers();
        receiveAllRepliesHandler(branchData, thisRequest, workers);
    }

    branchData->inCs = true;
    printf("proc %d enter to cs\n", branchData->id);
    fflush(stdout);
    return 0;
}

int release_cs(const void * self) {
    BranchData *branchData = (BranchData *) self;
    branchData->inCs = false;
    for (int i = 0; i <MAX_PROCESS_ID; ++i) {
        if (branchData->deferredRequests[branchData->id][i] == 1) {
            incrementLamportTime();
            Message reply;
            buildCsMessage(&reply, CS_REPLY);
            send(branchData, i, &reply);
            printf("in proc %d send reply to proc %d (in release)\n", branchData->id, i);
            fflush(stdout);
            branchData->deferredRequests[branchData->id][i] = 0;
        }
    }

    printf("proc %d out from cs\n", branchData->id);
    fflush(stdout);
    return 0;
}

Request sendCsRequest(BranchData *branchData) {
    incrementLamportTime();
    Message requestCsMsg;
    buildCsMessage(&requestCsMsg, CS_REQUEST);
    Workers workers = getWorkers();
    sendToAllWorkers(branchData, &requestCsMsg, &workers);
    printf("proc %d send request (%d, %d) to all workers\n", branchData->id, get_lamport_time(), branchData->id);
    fflush(stdout);
    Request request = {get_lamport_time(), branchData->id};
    return request;
}

void receiveAllRepliesHandler(BranchData *branchData, Request thisRequest, Workers workers) {
    Message csReplies;
    int ackCounter = 0;
    int currentWorkersLength = getWorkers().length ;
    int replies[MAX_PROCESS_ID];

    // cannot use empty init extension :(
    for (int i = 0; i < MAX_PROCESS_ID; ++i) {
        replies[i] = 0;
    }

    while (ackCounter < (currentWorkersLength - 1)) {
        if (receiveFromAnyWorkers(branchData, &csReplies) == 0) {
            if (csReplies.s_header.s_type == CS_REPLY) {
                ackCounter++;
                replies[branchData->senderId] = 1;
                printf("proc %d receive replay from proc %d\n", branchData->id, branchData->senderId);
                fflush(stdout);
            } else if (csReplies.s_header.s_type == CS_REQUEST) {
                receiveCsRequestAndSendReply(branchData, csReplies, thisRequest);
            } else if (csReplies.s_header.s_type == DONE) {
                deleteWorker(branchData->senderId);
                if (replies[branchData->senderId] == 0) {
                    ackCounter++;
                }
                printf("increase replyCounter for proc %d due to receive done msg\n", branchData->id);
                fflush(stdout);
            } else {
                printf("smth wrong\n");
            }
        }
    }
    printf("proc %d collect all replies for req (%d, %d)\n", branchData->id, thisRequest.time, thisRequest.procId);
    fflush(stdout);
}

void receiveCsRequestAndSendReply(BranchData *branchData, Message requestFromOther, Request thisRequest) {
    Request otherReq = {requestFromOther.s_header.s_local_time, branchData->senderId};

    printf("proc %d receive req (%d, %d) from proc %d\n", branchData->id, otherReq.time, otherReq.procId, branchData->senderId);
    fflush(stdout);
    if (branchData->inCs || (compare(thisRequest, otherReq) > 0)) {
        branchData->deferredRequests[branchData->id][branchData->senderId] = 1;
        printf("proc %d unapproved req (%d, %d) from proc %d\n", branchData->id, otherReq.time, otherReq.procId, branchData->senderId);
        fflush(stdout);
    } else {
        incrementLamportTime();
        Message replyMsg;
        buildCsMessage(&replyMsg, CS_REPLY);
        send(branchData, branchData->senderId, &replyMsg);
        branchData->deferredRequests[branchData->id][branchData->senderId] = 0;
        printf("proc %d send reply on req (%d, %d) to proc %d\n", branchData->id, otherReq.time, otherReq.procId, branchData->senderId);
        fflush(stdout);
    }
}





