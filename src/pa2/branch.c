#include "branch.h"
#include "pipes.h"
#include "messageUtils.h"

void createBranch(BranchData *branchData, const balance_t balances[]) {
    for (int i = 1; i < branchData->branchCount; ++i) {
        branchData->id = i;
        branchData->balance = balances[i];
        fflush(stdout);
        if (fork() == 0) {
            closeOtherChildDescriptors(branchData->descriptors, i, branchData->branchCount);
            run(branchData);
        }
    }
}


void run(BranchData *branchData) {
    char payload[MAX_PAYLOAD_LEN];

    Message startMessage;
    logStarted(branchData->id, payload, get_physical_time(), branchData->balance);
    buildEmptyMessage(&startMessage, payload, STARTED);
    send_multicast(branchData, &startMessage);

    Message startReceiver;
    receiveFromAll(branchData, &startReceiver);
    logReceiveStart(branchData->id, payload, get_physical_time());

    Message workMessage;
    int isWork = 1;
    while (isWork) {
        receive_any(branchData, &workMessage);
        if (workMessage.s_header.s_type == TRANSFER) {
            TransferOrder transferOrder;
            memcpy(&transferOrder, workMessage.s_payload, workMessage.s_header.s_payload_len);
            if (transferOrder.s_dst == branchData->id) {
                Message ackMessage;
                buildAckMessage(&ackMessage);
                send(branchData, PARENT_ID, &ackMessage);
            } else {
                send(branchData, transferOrder.s_dst, &workMessage);
            }
        } else if (workMessage.s_header.s_type == STOP) {
            isWork = 0;
        }
    }


    Message doneMessage;
    logDone(branchData->id, payload, get_physical_time(), branchData->balance);
    buildEmptyMessage(&doneMessage, payload, DONE);
    send_multicast(branchData, &doneMessage);

    Message doneReceiver;
    receiveFromAll(branchData, &doneReceiver);
    logReceiveDone(branchData->id, payload, get_physical_time());

    closePipes(branchData->descriptors, branchData->branchCount, branchData->id);
    exit(0);
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        wait(NULL);
    }
}
