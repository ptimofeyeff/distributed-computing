#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include "branch.h"
#include "message.h"
#include "history.h"
#include "logs.h"
#include "ipcx.h"

void createBranch(TopologyDescriptors *descriptors, const balance_t balances[], int branchCount) {
    for (int i = 1; i < branchCount; ++i) {
        BranchData branchData;
        branchData.id = i;
        branchData.balance = balances[i];
        branchData.descriptors = descriptors;
        branchData.branchCount = branchCount;
        if (fork() == 0) {
            closeOtherChildDescriptors(branchData.descriptors, i, branchCount);
            run(&branchData);
        }
    }
}


void run(BranchData *branchData) {
    char payload[MAX_PAYLOAD_LEN];
    int isWork = 1;
    timestamp_t lastCommitTime = 0;

    BalanceHistory balanceHistory;
    balanceHistory.s_id = branchData->id;

    BalanceState startBalance;
    buildBalanceState(&startBalance, branchData->balance);
    balanceHistory.s_history[0] = startBalance;

    Message startMessage;
    logStarted(branchData->id, get_physical_time(), branchData->balance);
    buildEmptyMessage(&startMessage, payload, STARTED);
    send_multicast(branchData, &startMessage);

    Message startMessages[branchData->branchCount];
    syncReceiveFromAllChild(branchData, startMessages);
    logReceiveStart(branchData->id, get_physical_time());


    while (isWork) {
        Message workMessage;
        receive_any(branchData, &workMessage);

        if (workMessage.s_header.s_type == TRANSFER) {
            TransferOrder transferOrder;
            memcpy(&transferOrder, workMessage.s_payload, workMessage.s_header.s_payload_len);
            if (transferOrder.s_src == branchData->id) {
                branchData->balance -= transferOrder.s_amount;
                BalanceState balanceState;
                buildBalanceState(&balanceState, branchData->balance);
                commitBalanceState(&balanceState, &balanceHistory, lastCommitTime, balanceState.s_time);
                lastCommitTime = balanceState.s_time;
                logTransferOut(get_physical_time(), branchData->id, transferOrder.s_amount, transferOrder.s_dst);
                send(branchData, transferOrder.s_dst, &workMessage);
            } else {
                branchData->balance += transferOrder.s_amount;
                BalanceState balanceState;
                buildBalanceState(&balanceState, branchData->balance);
                commitBalanceState(&balanceState, &balanceHistory, lastCommitTime, balanceState.s_time);
                lastCommitTime = balanceState.s_time;
                logTransferIn(get_physical_time(), transferOrder.s_src, transferOrder.s_amount, branchData->id);

                Message ackMessage;
                buildAckMessage(&ackMessage);
                send(branchData, PARENT_ID, &ackMessage);
            }
        } else if (workMessage.s_header.s_type == STOP) {
            isWork = 0;
            balanceHistory.s_history_len = get_physical_time() + 1;
            if (balanceHistory.s_history_len > (lastCommitTime + 1)) {
                BalanceState  finalState;
                buildBalanceState(&finalState, branchData->balance);
                commitBalanceState(&finalState, &balanceHistory, lastCommitTime, finalState.s_time);
            }
        }
    }

    Message doneMessage;
    buildEmptyMessage(&doneMessage, payload, DONE);
    logDone(branchData->id, get_physical_time(), branchData->balance);
    send_multicast(branchData, &doneMessage);

    Message doneMessages[branchData->branchCount];
    syncReceiveFromAllChild(branchData, doneMessages);
    logReceiveDone(branchData->id, get_physical_time());

    Message historyMessage;
    buildHistoryMessage(&historyMessage, &balanceHistory);
    send(branchData, PARENT_ID, &historyMessage);

    closePipes(branchData->descriptors, branchData->branchCount, branchData->id);
    exit(0);
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        wait(NULL);
    }
}

timestamp_t get_lamport_time() {
    return logicTime;
}
