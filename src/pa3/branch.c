#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include "branch.h"
#include "message.h"
#include "history.h"
#include "logs.h"
#include "ipcx.h"
#include "lamport.h"

BranchData branchData;
timestamp_t lastCommitTime = 0;
BalanceHistory balanceHistory;
int isWork = 1;

void init();
void start();
void work();
void done();
void finalize();


void createBranch(TopologyDescriptors *descriptors, const balance_t balances[], int branchCount) {
    for (int i = 1; i < branchCount; ++i) {
        branchData.id = i;
        branchData.balance = balances[i];
        branchData.descriptors = descriptors;
        branchData.branchCount = branchCount;
        branchData.logicTime = get_lamport_time();
        if (fork() == 0) {
            closeOtherChildDescriptors(branchData.descriptors, i, branchCount);
            run(&branchData);
        }
    }
}

void run() {
    init();
    start();
    work();
    done();
    finalize();
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        wait(NULL);
    }
}

void init() {
    balanceHistory.s_id = branchData.id;
    BalanceState initBalance;
    buildBalanceState(&initBalance, branchData.balance, 0);
    balanceHistory.s_history[0] = initBalance;
}

void start() {
    Message startMessage;
    incrementLamportTime();
    //logStarted(branchData.id, branchData.balance);
    buildStartMessage(&startMessage, branchData.id, branchData.balance);
    send_multicast(&branchData, &startMessage);

    Message startMessages[branchData.branchCount];
    incrementLamportTime();
    syncReceiveFromAllChild(&branchData, startMessages);
    //logReceiveStart(branchData.id);
}

void work() {
    while (isWork) {
        Message workMessage;
        receive_any(&branchData, &workMessage);

        if (workMessage.s_header.s_type == TRANSFER) {
            TransferOrder transferOrder;
            memcpy(&transferOrder, workMessage.s_payload, workMessage.s_header.s_payload_len);
            if (transferOrder.s_src == branchData.id) {
                branchData.balance -= transferOrder.s_amount;
                BalanceState balanceState;
                buildBalanceState(&balanceState, branchData.balance, transferOrder.s_amount);
                commitBalanceState(&balanceState, &balanceHistory, lastCommitTime);
                lastCommitTime = get_lamport_time();
                logTransferOut(branchData.id, transferOrder.s_amount, transferOrder.s_dst);

                incrementLamportTime();
                workMessage.s_header.s_local_time = get_lamport_time();
                send(&branchData, transferOrder.s_dst, &workMessage);
            } else {
                branchData.balance += transferOrder.s_amount;
                BalanceState balanceState;
                buildBalanceState(&balanceState, branchData.balance, 0);
                commitBalanceState(&balanceState, &balanceHistory, lastCommitTime);
                lastCommitTime = get_lamport_time();
                logTransferIn(transferOrder.s_src, transferOrder.s_amount, branchData.id);

                incrementLamportTime();
                Message ackMessage;
                buildAckMessage(&ackMessage);
                send(&branchData, PARENT_ID, &ackMessage);
            }
        } else if (workMessage.s_header.s_type == STOP) {
            isWork = 0;
            balanceHistory.s_history_len = get_lamport_time() + 1;
            if (balanceHistory.s_history_len > (lastCommitTime + 1)) {
                BalanceState  finalState;
                buildBalanceState(&finalState, branchData.balance, 0);
                commitBalanceState(&finalState, &balanceHistory, lastCommitTime);
            }
        }
    }
}

void done() {
    Message doneMessage;
    incrementLamportTime();
    buildDoneMessage(&doneMessage, branchData.id, branchData.balance);
    //logDone(branchData.id, branchData.balance);
    send_multicast(&branchData, &doneMessage);

    Message doneMessages[branchData.branchCount];
    syncReceiveFromAllChild(&branchData, doneMessages);
    //logReceiveDone(branchData.id);
}

void finalize() {
    Message historyMessage;
    incrementLamportTime();
    buildHistoryMessage(&historyMessage, &balanceHistory);
    send(&branchData, PARENT_ID, &historyMessage);
    closePipes(branchData.descriptors, branchData.branchCount, branchData.id);
    exit(0);
}

