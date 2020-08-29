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

    if (getWorkers().length > 1) {
        Workers  workers = getWorkers();
        receiveAllRepliesHandler(branchData, currentRequest, workers);
    }

    while (checkEnterCondition(branchData, currentRequest) != 0) {
        //printf("proc %d wait for approve\n", branchData->id);
        syncReceiveCs(branchData);
    }
    return 0;
}

int release_cs(const void * self) {
    BranchData *branchData = (BranchData *) self;
    dequeue();
    //printf("in proc %d request (%d, %d) was deleted\n", branchData->id, request.time, request.procId);
    fflush(stdout);

    incrementLamportTime();
    Message releaseMsg;
    buildCsMessage(&releaseMsg, CS_RELEASE);
    Workers workers = getWorkers();
    sendToAllWorkers(branchData, &releaseMsg, &workers);
    //printf("in proc %d send to delete (%d, %d)\n", branchData->id, request.time, request.procId);
    fflush(stdout);
    return 0;
}



void syncReceiveCs(BranchData *branchData) {
    Message requestFromOther;
    while (true) {
        if (receive_any(branchData, &requestFromOther) == 0) {
            if (requestFromOther.s_header.s_type == CS_REQUEST) {
                receiveCsRequestAndSendReply(branchData, requestFromOther);
            } else if (requestFromOther.s_header.s_type == CS_RELEASE) {
                receiveCsRelease(branchData, requestFromOther);
            } else if (requestFromOther.s_header.s_type == DONE) {
                deleteWorker(branchData->senderId);
                //printf("in proc %d delete worker %d\n", branchData->id, branchData->senderId);
                fflush(stdout);
            } else {
                //printf("smth wrong\n");
                fflush(stdout);
            }
            return;
        }
    }
}

Request sendAndSaveCsRequest(BranchData *branchData) {
    incrementLamportTime();
    Message requestCsMsg;
    buildCsMessage(&requestCsMsg, CS_REQUEST);
    Workers workers = getWorkers();
    sendToAllWorkers(branchData, &requestCsMsg, &workers);
    //printf("proc %d send request (%d, %d)\n", branchData->id, get_lamport_time(), branchData->id);
    fflush(stdout);

    Request request = {get_lamport_time(), branchData->id};
    enqueue(request);
    //printf("proc %d enqueue request (%d, %d)\n", branchData->id, request.time, request.procId);
    fflush(stdout);
    return request;
}

void receiveAllRepliesHandler(BranchData *branchData, Request currentRequest, Workers workers) {
    Message csReplies;
    int ackCounter = 0;
    int currentWorkersLength = getWorkers().length;

    for (int i = 0; i < workers.length; ++i) {
        if (branchData->id != workers.procId[i]) {
            while (ackCounter < currentWorkersLength) {
                //printf("in proc %d try to receive reply for req (%d, %d) from proc %d\n",
                  //     branchData->id, currentRequest.time, currentRequest.procId, workers.procId[i]);
                fflush(stdout);
                if (receiveFromAnyWorkers(branchData, &csReplies) == 0) {

                    if (csReplies.s_header.s_type == CS_REPLY) {
                        if (csReplies.s_header.s_local_time > currentRequest.time) {
                            //printf("proc %d receive ack from proc %d for request (%d, %d)\n",
                              //     branchData->id, branchData->senderId, currentRequest.time, currentRequest.procId);
                            fflush(stdout);
                            ackCounter++;
                            break;
                        } else {
                            printf("wrong reply\n");
                            continue;
                        }
                    } else if (csReplies.s_header.s_type == CS_REQUEST) {
                        receiveCsRequestAndSendReply(branchData, csReplies);
                        continue;
                    } else if (csReplies.s_header.s_type == CS_RELEASE) {
                        receiveCsRelease(branchData, csReplies);
                        continue;
                    } else if (csReplies.s_header.s_type == DONE) {
                        deleteWorker(branchData->senderId);
                        //printf("in proc %d delete worker %d\n", branchData->id, branchData->senderId);
                        fflush(stdout);
                        ackCounter++;
                        break;
                    } else {
                        printf("smth wrong\n");
                    }
                }
            }
        }
    }
}

int checkEnterCondition(BranchData *branchData, Request currentRequest) {

    Request firstRequest = peek();

    if (compare(firstRequest, currentRequest) == 0) {
        //printf("in proc %d approved the request (%d, %d)\n", branchData->id, currentRequest.time, currentRequest.procId);
        fflush(stdout);
        return 0;
    } else {
        //printf("in proc %d unapproved the request (%d, %d)\n", branchData->id, currentRequest.time, currentRequest.procId);
        fflush(stdout);
        return -1;
    }
}

void receiveCsRequestAndSendReply(BranchData *branchData, Message requestFromOther) {
    Request request = {requestFromOther.s_header.s_local_time, branchData->senderId};
    //printf("proc %d receive req (%d, %d) from proc %d\n",
   //        branchData->id, request.time, request.procId,  branchData->senderId);
    fflush(stdout);

    enqueue(request);
    //printf("proc %d enqueue request (%d, %d)\n", branchData->id, request.time, request.procId);
    fflush(stdout);

    incrementLamportTime();
    Message replyMsg;
    buildCsMessage(&replyMsg, CS_REPLY);
    send(branchData, branchData->senderId, &replyMsg);
    //printf("proc %d send reply for (%d, %d) to proc %d\n", branchData->id, request.time, request.procId, branchData->senderId);
    fflush(stdout);
}

void receiveCsRelease(BranchData *branchData, Message release) {
    //Request request = {release.s_header.s_local_time, branchData->senderId};

    //printf("proc %d receive release msg from proc %d\n", branchData->id, branchData->senderId);
    fflush(stdout);
    dequeue(); // тут нужно удалить именно тот, который пришел, а не первый
    //printf("proc %d delete request (%d, %d)\n", branchData->id, request.time, request.procId);
    fflush(stdout);
}




