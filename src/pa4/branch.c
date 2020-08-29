#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include "branch.h"
#include "message.h"
#include "logs.h"
#include "ipcx.h"
#include "lamport.h"
#include "pa2345.h"

BranchData branchData;
int isWork = 1;

void init();
void start();
void work();
void done();
void finalize();


void createBranch(TopologyDescriptors *descriptors, int branchCount) {
    for (int i = 1; i < branchCount; ++i) {
        branchData.id = i;
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
}

void start() {
    Message startMessage;
    incrementLamportTime();
    logStarted(branchData.id, 0);
    buildStartMessage(&startMessage, branchData.id, 0);
    send_multicast(&branchData, &startMessage);

    Message startMessages[branchData.branchCount];
    syncReceiveFromAllChild(&branchData, startMessages);
    logReceiveStart(branchData.id);
}

void work() {
    initWorkers(branchData.branchCount);

    int loopCount = branchData.id * 3;

    for (int i = 1; i <= loopCount; ++i) {
        char message[256];
        sprintf(message, log_loop_operation_fmt, branchData.id, i, loopCount);
        request_cs(&branchData);

        printf("%s", message);
        print(message);
        fflush(stdout);

        release_cs(&branchData);
    }
}

void done() {
    incrementLamportTime();
    Message doneMessage;
    buildDoneMessage(&doneMessage, branchData.id, 0);
    logDone(branchData.id, 0);
    send_multicast(&branchData, &doneMessage);

    Message doneMessages[branchData.branchCount];
    Workers workers = getWorkers();
    syncReceiveDoneFromAllWorkers(&branchData, doneMessages, &workers);
    logReceiveDone(branchData.id);
}

void finalize() {
    closePipes(branchData.descriptors, branchData.branchCount, branchData.id);
    exit(0);
}

