#include "branch.h"
#include "pipes.h"

void createBranch(BranchData *branchData, const balance_t balances[]) {
    for (int i = 1; i < branchData->branchCount; ++i) {
        branchData->id = i;
        branchData->balance = balances[i];
        fflush(stdout);
        if (fork() == 0) {
            closeOtherChildDescriptors(&branchData->descriptors, i, branchData->branchCount);
            run(branchData);
        }
    }
}


void run(BranchData *branchData) {
    char payload[MAX_PAYLOAD_LEN];

    Message startMessage;
    logStarted(branchData->id, payload, get_physical_time(), branchData->balance);
    buildMessage(&startMessage, payload, STARTED);
    send_multicast(branchData, &startMessage);

    Message startReceiver;
    receiveMessages(branchData, &startReceiver);
    logReceiveStart(branchData->id, payload, get_physical_time());

    // TODO полезная работа

    Message doneMessage;
    logDone(branchData->id, payload, get_physical_time(), branchData->balance);
    buildMessage(&doneMessage, payload, DONE);
    send_multicast(branchData, &doneMessage);

    Message doneReceiver;
    receiveMessages(branchData, &doneReceiver);
    logReceiveDone(branchData->id, payload, get_physical_time());

    closePipes(&branchData->descriptors, branchData->branchCount, branchData->id);
    exit(0);
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        wait(NULL);
    }
}

void buildMessage(Message *message, char *payload, MessageType type) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = type;
    message->s_header.s_local_time = time(NULL);
    message->s_header.s_payload_len = strlen(payload);
    strcpy(message->s_payload, payload);
}

void receiveMessages(BranchData *branchData, Message *message) {
    for (int i = 1; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            receive(branchData, i, message);
            //printMessage(message, metaData->localId);
        }
    }
}
