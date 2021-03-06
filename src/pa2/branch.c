#include "branch.h"
#include "descriptors.h"
#include "message.h"

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
    int isWork = 1;
    timestamp_t lastCommitTime = 0;

    BalanceHistory balanceHistory;
    balanceHistory.s_id = branchData->id;

    BalanceState startBalance;
    buildBalanceState(&startBalance, branchData->balance);
    balanceHistory.s_history[0] = startBalance;

    Message startMessage;
    logStarted(branchData->id, payload, get_physical_time(), branchData->balance);
    buildEmptyMessage(&startMessage, payload, STARTED);
    send_multicast(branchData, &startMessage);

    Message startReceiver;
    receiveFromAll(branchData, &startReceiver);
    logReceiveStart(branchData->id, payload, get_physical_time());


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
    logDone(branchData->id, payload, get_physical_time(), branchData->balance);
    buildEmptyMessage(&doneMessage, payload, DONE);
    send_multicast(branchData, &doneMessage);

    Message doneReceiver;
    receiveFromAll(branchData, &doneReceiver);
    logReceiveDone(branchData->id, payload, get_physical_time());

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
