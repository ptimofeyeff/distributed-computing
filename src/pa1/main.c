#include "main.h"
#include "logs.h"
#include "pipes.h"


void createChild(MetaData *, ProcessPipes []);

void run(MetaData *);

void waitChild(int count);

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");
    pipesLogs = fopen(pipes_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    int procCount = cpCount + 1;

    MetaData metaData;
    metaData.procCount = procCount;
    metaData.localId = PARENT_ID;

    ProcessPipes processesPipes[procCount];

    initParentPipes(&processesPipes[0], procCount);

    for (int i = 1; i <procCount; ++i) {
        initChildPipes(&processesPipes[i], procCount, i);
    }

    createChild(&metaData, processesPipes);

    Message message;

    // receive started
    for (int i = 1; i <= cpCount; ++i) {
        receive(&metaData, i, &message);
        //printMessage(&message, 0);
    }

    // receive done
    for (int i = 1; i <= cpCount; ++i) {
        receive(&metaData, i, &message);
        //printMessage(&message, 0);
    }

    waitChild(cpCount);
    closeParentPipes(&processesPipes[0], procCount);
    fclose(pipesLogs);
    return 0;
}


void createChild(MetaData *metaData, ProcessPipes processesPipes[]) {
    for (int i = 1; i < metaData->procCount; ++i) {
        metaData->localId = i;
        metaData->pipesData = processesPipes[i];
        fflush(stdout);
        if (fork() == 0) {
            run(metaData);
        }
    }
}


void run(MetaData *metaData) {

    char payload[MAX_PAYLOAD_LEN];
    logStarted(metaData->localId, payload);

    Message startSender;
    startSender.s_header.s_magic = MESSAGE_MAGIC;
    startSender.s_header.s_type = STARTED;
    startSender.s_header.s_local_time = time(NULL);
    startSender.s_header.s_payload_len = strlen(payload);
    strcpy(startSender.s_payload, payload);


    send_multicast(metaData, &startSender);

    Message startReceivers;

    for (int i = 1; i < metaData->procCount; ++i) {
        if (i != metaData->localId) {
            receive(metaData, i, &startReceivers);
        }
    }

    logReceiveStart(metaData->localId, payload);

    Message doneSender;
    logDone(metaData->procCount, payload);

    doneSender.s_header.s_type = DONE;
    doneSender.s_header.s_local_time = time(NULL);
    doneSender.s_header.s_payload_len = strlen(payload);
    strcpy(doneSender.s_payload, payload);

    send_multicast(metaData, &doneSender);

    Message doneReceiver;

    for (int i = 1; i < metaData->procCount; ++i) {
        if (i != metaData->localId) {
            receive(metaData, i, &doneReceiver);
        }
    }

    logReceiveDone(metaData->localId, payload);
    closeChildPipes(&metaData->pipesData, metaData->procCount, metaData->localId);

    exit(0);
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        wait(NULL);
    }
}
