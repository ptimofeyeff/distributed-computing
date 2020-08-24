#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "common.h"
#include "logs.h"
#include "branch.h"
#include "ipcx.h"
#include "lamport.h"

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");
    pipesLogs = fopen(pipes_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    int procCount = cpCount + 1;

    openPipes(&topologyDescriptors, procCount);

    createBranch(&topologyDescriptors, procCount);
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

    Message receiveMessages[procCount];
    syncReceiveFromAllChild(&mainBranch, receiveMessages);
    logReceiveDone(PARENT_ID);

    waitChild(cpCount);
    closePipes(&topologyDescriptors, procCount, PARENT_ID);
    fclose(pipesLogs);
    return 0;
}
