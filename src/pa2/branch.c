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

    Message startMessage;
    logStarted(branchData->id, payload, get_physical_time(), branchData->balance);
    buildEmptyMessage(&startMessage, payload, STARTED);
    send_multicast(branchData, &startMessage);

    Message startReceiver;
    receiveFromAll(branchData, &startReceiver);
    logReceiveStart(branchData->id, payload, get_physical_time());

    printf("PROC %d START WORKING----------------------------------------------\n", branchData->id);
    fflush(stdout);

    BalanceHistory balanceHistory;
    balanceHistory.s_id = branchData->id;
    BalanceState startBalance;
    startBalance.s_balance = branchData->balance;
    startBalance.s_balance_pending_in = 0;
    startBalance.s_time = get_physical_time();
    balanceHistory.s_history[0] = startBalance; // затем индекс можно сделать функцией get_physical_time();

    int isWork = 1;
    int workCounter = 1;

    while (isWork) {
        Message workMessage;
        receive_any(branchData, &workMessage);
        if (workMessage.s_header.s_type == TRANSFER) {
            TransferOrder transferOrder;
            memcpy(&transferOrder, workMessage.s_payload, workMessage.s_header.s_payload_len);
            if (transferOrder.s_src == branchData->id) {

                branchData->balance -= transferOrder.s_amount;
                logTransferOut(get_physical_time(), branchData->id, transferOrder.s_amount, transferOrder.s_dst);

                BalanceState balanceState;
                balanceState.s_balance_pending_in = 0;
                balanceState.s_balance = branchData->balance;
                balanceState.s_time = get_physical_time();
                balanceHistory.s_history[workCounter] = balanceState;

                send(branchData, transferOrder.s_dst, &workMessage);
            } else {

                branchData->balance += transferOrder.s_amount;
                logTransferIn(get_physical_time(), transferOrder.s_src, transferOrder.s_amount, branchData->id);

                BalanceState balanceState;
                balanceState.s_balance_pending_in = 0;
                balanceState.s_balance = branchData->balance;
                balanceState.s_time = get_physical_time();
                balanceHistory.s_history[workCounter] = balanceState;
                Message ackMessage;
                buildAckMessage(&ackMessage);
                send(branchData, PARENT_ID, &ackMessage);

            }
        } else if (workMessage.s_header.s_type == STOP) {
            isWork = 0;
        }
        workCounter++;
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
    send(&branchData, PARENT_ID, &historyMessage);

    closePipes(branchData->descriptors, branchData->branchCount, branchData->id);
    exit(0);
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        wait(NULL);
    }
}
