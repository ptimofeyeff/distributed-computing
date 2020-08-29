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

    int cpCount;
    int procCount;

    if (argc == 3) {
        cpCount = (int) strtol(argv[2], NULL, 10);
        procCount = cpCount + 1;
        openPipes(&topologyDescriptors, procCount);
        createBranch(&topologyDescriptors, procCount, false);
    } else {
        cpCount = (int) strtol(argv[3], NULL, 10);
        procCount = cpCount + 1;
        openPipes(&topologyDescriptors, procCount);
        createBranch(&topologyDescriptors, procCount, true);
    }

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
