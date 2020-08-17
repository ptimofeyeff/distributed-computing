#include <stdlib.h>
#include <string.h>

#include "pa23.h"
#include "common.h"
#include "logs.h"
#include "branch.h"
#include "message.h"
#include "ipcx.h"

void transfer(void *parent_data, local_id src, local_id dst, balance_t amount) {
    Message transfer;

    TransferOrder transferOrder;
    transferOrder.s_src = src;
    transferOrder.s_dst = dst;
    transferOrder.s_amount = amount;
    buildTransferMessage(&transfer, &transferOrder);

    send(parent_data, src, &transfer);
    Message ackMessage;


    while (1) {
        if (receive(parent_data, dst, &ackMessage) == 0) {
            break;
        }
    }
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
    mainBranch.logicTime = 0;

    Message startMessages[procCount];
    syncReceiveFromAllChild(&mainBranch, startMessages);
    logReceiveStart(PARENT_ID, get_physical_time());

    bank_robbery(&mainBranch, cpCount);

    Message stopMessage;
    buildStopMessage(&stopMessage);
    send_multicast(&mainBranch, &stopMessage);

    // receive done
    Message receiveMessages[procCount];
    syncReceiveFromAllChild(&mainBranch, receiveMessages);
    logReceiveDone(PARENT_ID, get_physical_time());

    Message balanceMsg[procCount];


    for (int i = 1; i < procCount; ++i) {
        while (1) {
            if (receive(&mainBranch, i, &balanceMsg[i]) == 0) {
                break;
            }
        }
    }

    AllHistory allHistory;
    allHistory.s_history_len = cpCount;
    for (int i = 0; i < cpCount; ++i) {
        BalanceHistory balanceHistory;
        memcpy(&balanceHistory, &balanceMsg[i + 1].s_payload, sizeof(BalanceHistory));
        allHistory.s_history[i] = balanceHistory;
    }

    print_history(&allHistory);
    waitChild(cpCount);
    closePipes(&topologyDescriptors, procCount, PARENT_ID);
    fclose(pipesLogs);
    return 0;
}
