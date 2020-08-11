#include "pa23.h"

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {
    Message transfer;

    TransferOrder transferOrder;
    transferOrder.s_src = src;
    transferOrder.s_dst = dst;
    transferOrder.s_amount = amount;
    buildTransferMessage(&transfer, &transferOrder);

    send(parent_data, src, &transfer);
    logTransferOut(get_physical_time(), src, amount, dst, transfer.s_payload);
    Message ackMessage;

    while (1) {
        if (receive(parent_data, dst, &ackMessage) == 0) {
            break;
        }
    }
    printf("receive ack from %d", dst);
    fflush(stdout);

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

    BranchDescriptors branchDescriptors;
    openPipes(&branchDescriptors, procCount);
    setNonBlocking(&branchDescriptors, procCount);

    BranchData branchData;
    branchData.branchCount = procCount;
    branchData.descriptors = &branchDescriptors;

    createBranch(&branchData, branchBalances);
    closeOtherParentDescriptors(branchData.descriptors, procCount);

    branchData.id = PARENT_ID;
    branchData.descriptors = &branchDescriptors; // may redundant line

    Message message;

    // receive started
    receiveFromAll(&branchData, &message);
    logReceiveStart(PARENT_ID, message.s_payload, get_physical_time());

    bank_robbery(&branchData, cpCount);

    Message stopMessage;

    buildStopMessage(&stopMessage);

    send_multicast(&branchData, &stopMessage);

    // receive done
    receiveFromAll(&branchData, &message);

    // TODO: receive balance history from every branch and call print_history()


    waitChild(cpCount);
    closePipes(&branchDescriptors, procCount, PARENT_ID);
    fclose(pipesLogs);
    return 0;
}
