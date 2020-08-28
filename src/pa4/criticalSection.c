#include "criticalSection.h"
#include "pa2345.h"
#include "lamport.h"
#include "queue.h"
#include "message.h"
#include <stdio.h>
#include "ipcx.h"

int request_cs(const void * self) {
    BranchData *branchData = (BranchData *) self;

    Request currentRequest = sendAndSaveCsRequest(branchData);
    syncReceiveCsReplies(branchData, currentRequest);

    while (checkEnterCondition(branchData, currentRequest) != 0) {
        syncReceiveCs(branchData);
    }
    return 0;
}

int release_cs(const void * self) {
    BranchData *branchData = (BranchData *) self;
    Request request = dequeue();
    printf("in proc %d request (%d, %d) was deleted\n", branchData->id, request.time, request.procId);

    incrementLamportTime();
    Message releaseMsg;
    buildCsMessage(&releaseMsg, CS_RELEASE);
    sendToAllChild(branchData, &releaseMsg);
    return 0;
}



void syncReceiveCs(BranchData *branchData) {
    Message requestFromOther;
    while (true) {
        if (receive_any(branchData, &requestFromOther) == 0) {
            printf("proc %d receive message from proc %d with type %d\n", branchData->id, branchData->senderId, requestFromOther.s_header.s_type);
            if (requestFromOther.s_header.s_type == CS_REQUEST) {
                receiveCsRequestAndSendReply(branchData, requestFromOther);
            }
            if (requestFromOther.s_header.s_type == CS_RELEASE) {
                receiveCsRelease(branchData, requestFromOther);
            }
            return;
        }
    }
}

Request sendAndSaveCsRequest(BranchData *branchData) {
    incrementLamportTime();
    Message requestCsMsg;
    buildCsMessage(&requestCsMsg, CS_REQUEST);
    sendToAllChild(branchData, &requestCsMsg);
    printf("proc %d send request (%d, %d)\n", branchData->id, get_lamport_time(), branchData->id);

    Request request = {get_lamport_time(), branchData->id};
    enqueue(request);
    printf("proc %d enqueue request (%d, %d)\n", branchData->id, request.time, request.procId);
    return request;
}

void syncReceiveCsReplies(BranchData *branchData, Request currentRequest) {
    Message csReplies[branchData->branchCount];
    for (int i = 1; i < branchData->branchCount; ++i) {
        if (branchData->id != i) {
            while (true) {
                if (receive(branchData, i, &csReplies[i]) == 0) { // голый ресив не увеличивает отметку времени при приеме
                    printf("proc %d receive message from proc %d with type %d\n",
                           branchData->id, branchData->senderId, csReplies[i].s_header.s_type);
                    if (branchData->logicTime < csReplies[i].s_header.s_local_time) {
                        setLamportTime(csReplies[i].s_header.s_local_time);
                    }
                    incrementLamportTime();

                    if (csReplies[i].s_header.s_type == CS_REPLY) {
                        if (csReplies[i].s_header.s_local_time > currentRequest.time) {
                            break;
                        } else {
                            continue;
                        }
                    } else {
                        if (csReplies[i].s_header.s_type == CS_REQUEST) {
                            receiveCsRequestAndSendReply(branchData, csReplies[i]);
                            continue;
                        }
                        if (csReplies[i].s_header.s_type == CS_RELEASE) {
                            receiveCsRelease(branchData, csReplies[i]);
                            continue;
                        }
                    }
                }
            }
        }
    }
}

int checkEnterCondition(BranchData *branchData, Request currentRequest) {

    Request firstRequest = peek();

    if (compare(firstRequest, currentRequest) == 0) {
        printf("in proc %d approved the request (%d, %d)\n", branchData->id, currentRequest.time, currentRequest.procId);
        return 0;
    } else {
        printf("in proc %d unapproved the request (%d, %d)\n", branchData->id, currentRequest.time, currentRequest.procId);
        return -1;
    }
}

void receiveCsRequestAndSendReply(BranchData *branchData, Message requestFromOther) {
    Request request = {requestFromOther.s_header.s_local_time, branchData->senderId};
    enqueue(request);
    printf("proc %d enqueue request (%d, %d)\n", branchData->id, request.time, request.procId);

    incrementLamportTime();
    Message replyMsg;
    buildCsMessage(&replyMsg, CS_REPLY);
    send(branchData, branchData->senderId, &replyMsg);
    printf("proc %d send reply message to proc %d\n", branchData->id, branchData->senderId);
}

void receiveCsRelease(BranchData *branchData, Message release) {
    //Request request = {release.s_header.s_local_time, branchData->senderId};

    Request request = dequeue(); // тут нужно удалить именно тот, который пришел, а не первый
    printf("proc %d delete request (%d, %d)\n", branchData->id, request.time, request.procId);
}




