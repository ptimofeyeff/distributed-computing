#include "pa23.h"

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {
    Message transfer;

    TransferOrder transferOrder;
    transferOrder.s_amount = amount;
    transferOrder.s_src = src;
    transferOrder.s_dst = dst;
    buildTransferMessage(&transfer, &transferOrder);

    BranchData branchData;
    BranchDescriptors *parentDescriptors = (BranchDescriptors *) parent_data;
    branchData.descriptors = parentDescriptors;
    branchData.id = src;

    send(&branchData, dst, &transfer);
    logTransferOut(get_physical_time(), src, amount, dst, transfer.s_payload);
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

    BranchDescriptors branchDescriptors; // матрица дескрипторов
    openPipes(&branchDescriptors, procCount);

    BranchData branchData;
    branchData.branchCount = procCount;
    branchData.descriptors = &branchDescriptors;

    createBranch(&branchData, branchBalances);
    closeOtherParentDescriptors(branchData.descriptors, procCount);

    branchData.id = PARENT_ID;
    branchData.descriptors = &branchDescriptors;

    Message message;

    // receive started
    receiveMessages(&branchData, &message);

    bank_robbery(&branchData.descriptors, cpCount);

    // receive ACK from Cdst

    //TODO: send STOP to every branches

    // receive done
    receiveMessages(&branchData, &message);

    // TODO: receive balance history from every branch and call print_history()


    waitChild(cpCount);
    closePipes(&branchDescriptors, procCount, PARENT_ID);
    fclose(pipesLogs);
    return 0;
}
