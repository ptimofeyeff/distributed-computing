#include <stdlib.h>
#include <string.h>

#include "pa23.h"
#include "common.h"
#include "logs.h"
#include "branch.h"
#include "message.h"
#include "ipcx.h"
#include "lamport.h"

typedef struct {
    local_id ackProc;
    timestamp_t ackTime;
    balance_t amount;
} AckMessage;


typedef struct {
    BranchData *mainBranch;
    int ackMessageLength;
    AckMessage ackMessages[4096];
} MainTransfer;

void transfer(void *parent_data, local_id src, local_id dst, balance_t amount) {
    MainTransfer *mainTransfer = (MainTransfer *) parent_data;

    TransferOrder transferOrder;
    transferOrder.s_src = src;
    transferOrder.s_dst = dst;
    transferOrder.s_amount = amount;

    incrementLamportTime();
    Message transfer;
    buildTransferMessage(&transfer, &transferOrder);
    send(mainTransfer->mainBranch, src, &transfer);

    Message ackMessage;
    syncReceive(mainTransfer->mainBranch, dst, &ackMessage);

    mainTransfer->ackMessageLength++;
    mainTransfer->ackMessages[mainTransfer->ackMessageLength].ackTime = ackMessage.s_header.s_local_time - 1;
    mainTransfer->ackMessages[mainTransfer->ackMessageLength].ackProc = src;
    mainTransfer->ackMessages[mainTransfer->ackMessageLength].amount = amount;
}

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");
    pipesLogs = fopen(pipes_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    balance_t branchBalances[cpCount];
    for (int i = 1; i <= cpCount; ++i) {
        branchBalances[i] = strtol(argv[i + 2], NULL, 10);
    }
    int procCount = cpCount + 1;

    openPipes(&topologyDescriptors, procCount);

    createBranch(&topologyDescriptors, branchBalances, procCount);
    closeOtherParentDescriptors(&topologyDescriptors, procCount);

    BranchData mainBranch;
    mainBranch.id = PARENT_ID;
    mainBranch.branchCount = procCount;
    mainBranch.descriptors = &topologyDescriptors;
    mainBranch.logicTime = get_lamport_time();

    incrementLamportTime(); // starting emulation?

    Message startMessages[procCount];
    syncReceiveFromAllChild(&mainBranch, startMessages);
    logReceiveStart(PARENT_ID);

    MainTransfer mainTransfer;
    mainTransfer.mainBranch = &mainBranch;
    mainTransfer.ackMessageLength = -1;
    bank_robbery(&mainTransfer, cpCount);

    incrementLamportTime();
    Message stopMessage;
    buildStopMessage(&stopMessage);
    send_multicast(&mainBranch, &stopMessage);

    Message receiveMessages[procCount];
    syncReceiveFromAllChild(&mainBranch, receiveMessages);
    logReceiveDone(PARENT_ID);

    Message balanceMessages[procCount];
    syncReceiveFromAllChild(&mainBranch, balanceMessages);

    AllHistory allHistory;
    allHistory.s_history_len = cpCount;
    for (int i = 0; i < allHistory.s_history_len; ++i) {
        BalanceHistory balanceHistory;
        memcpy(&balanceHistory, &balanceMessages[i + 1].s_payload, sizeof(BalanceHistory));
        allHistory.s_history[i] = balanceHistory;
    }

    for (int i = 0; i <= mainTransfer.ackMessageLength; ++i) {
        timestamp_t ackTime = mainTransfer.ackMessages[i].ackTime;
        local_id  proc = mainTransfer.ackMessages[i].ackProc;
        balance_t amount = mainTransfer.ackMessages[i].amount;
        for (int j = ackTime; j <allHistory.s_history[proc -1].s_history_len ; ++j) {
            allHistory.s_history[proc - 1].s_history[j].s_balance_pending_in -= amount;
        }
    }

    print_history(&allHistory);
    waitChild(cpCount);
    closePipes(&topologyDescriptors, procCount, PARENT_ID);
    fclose(pipesLogs);
    return 0;
}
