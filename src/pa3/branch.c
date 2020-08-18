#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include "branch.h"
#include "message.h"
#include "history.h"
#include "logs.h"
#include "ipcx.h"

BranchData branchData;
timestamp_t logicTime = 0;
timestamp_t lastCommitTime = 0;
BalanceHistory balanceHistory;
int isWork = 1;

void init();
void start();
void done();
void work();
void finalize();


void createBranch(TopologyDescriptors *descriptors, const balance_t balances[], int branchCount) {
    for (int i = 1; i < branchCount; ++i) {
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

timestamp_t get_lamport_time() {
    return get_physical_time();
    //return logicTime;
}

void init() {
    balanceHistory.s_id = branchData.id;
    BalanceState initBalance;
    buildBalanceState(&initBalance, branchData.balance);
    balanceHistory.s_history[0] = initBalance;
}

void start() {
    Message startMessage;
    logicTime++;
    logStarted(branchData.id, branchData.balance);
    buildStartMessage(&startMessage, branchData.id, branchData.balance);
    send_multicast(&branchData, &startMessage);

    Message startMessages[branchData.branchCount];
    logicTime++;
    syncReceiveFromAllChild(&branchData, startMessages);
    logReceiveStart(branchData.id);
}

void work() {
    while (isWork) {
        Message workMessage;
        logicTime++;
        receive_any(&branchData, &workMessage);

        if (workMessage.s_header.s_type == TRANSFER) {
            TransferOrder transferOrder;
            memcpy(&transferOrder, workMessage.s_payload, workMessage.s_header.s_payload_len);
            if (transferOrder.s_src == branchData.id) {
                branchData.balance -= transferOrder.s_amount;
                BalanceState balanceState;
                buildBalanceState(&balanceState, branchData.balance);
                commitBalanceState(&balanceState, &balanceHistory, lastCommitTime, balanceState.s_time);
                lastCommitTime = balanceState.s_time;
                logicTime++;
                logTransferOut(branchData.id, transferOrder.s_amount, transferOrder.s_dst);
                send(&branchData, transferOrder.s_dst, &workMessage);
            } else {
                branchData.balance += transferOrder.s_amount;
                BalanceState balanceState;
                buildBalanceState(&balanceState, branchData.balance);
                commitBalanceState(&balanceState, &balanceHistory, lastCommitTime, balanceState.s_time);
                lastCommitTime = balanceState.s_time;
                logicTime++;
                logTransferIn(transferOrder.s_src, transferOrder.s_amount, branchData.id);
                Message ackMessage;
                buildAckMessage(&ackMessage);
                send(&branchData, PARENT_ID, &ackMessage);
            }
        } else if (workMessage.s_header.s_type == STOP) {
            isWork = 0;
            balanceHistory.s_history_len = get_lamport_time() + 1;
            if (balanceHistory.s_history_len > (lastCommitTime + 1)) {
                BalanceState  finalState;
                buildBalanceState(&finalState, branchData.balance);
                commitBalanceState(&finalState, &balanceHistory, lastCommitTime, finalState.s_time);
            }
        }
    }
}

void done() {
    Message doneMessage;
    logicTime++;
    buildDoneMessage(&doneMessage, branchData.id, branchData.balance);
    logDone(branchData.id, branchData.balance);
    send_multicast(&branchData, &doneMessage);

    Message doneMessages[branchData.branchCount];
    logicTime++;
    syncReceiveFromAllChild(&branchData, doneMessages);
    logReceiveDone(branchData.id);
}

void finalize() {
    Message historyMessage;
    logicTime++;
    buildHistoryMessage(&historyMessage, &balanceHistory);
    send(&branchData, PARENT_ID, &historyMessage);
    closePipes(branchData.descriptors, branchData.branchCount, branchData.id);
    exit(0);
}

