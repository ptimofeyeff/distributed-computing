#include "main.h"
#include "logs.h"
#include "pipes.h"

void buildMessage(Message *, char *, MessageType);

void receiveMessages(MetaData *, Message *);

void createChild(MetaData *);

void run(MetaData *);

void waitChild(int count);

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");
    pipesLogs = fopen(pipes_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    int procCount = cpCount + 1;

    ProcessPipes processesPipes;
    openPipes(&processesPipes, procCount); // инициализированная полная матрица пайпов

    MetaData metaData;
    metaData.procCount = procCount;
    metaData.pipesData = processesPipes;

    createChild(&metaData);

    Message message;

    // receive started
    metaData.localId = PARENT_ID;
    metaData.pipesData = processesPipes;
    receiveMessages(&metaData, &message);

    // receive done
    receiveMessages(&metaData, &message);

    waitChild(cpCount);
    closePipes(&processesPipes, procCount, PARENT_ID);
    fclose(pipesLogs);
    return 0;
}


void createChild(MetaData *metaData) {
    for (int i = 1; i < metaData->procCount; ++i) {
        metaData->localId = i;
        fflush(stdout);
        if (fork() == 0) {
            closeOtherChildDescriptors(&metaData->pipesData, i, metaData->procCount);
            run(metaData);
        } else {
            closeOtherParentDescriptors(&metaData->pipesData, metaData->procCount);
        }
    }
}


void run(MetaData *metaData) {
    char payload[MAX_PAYLOAD_LEN];

    Message startMessage;
    logStarted(metaData->localId, payload);
    buildMessage(&startMessage, payload, STARTED);
    send_multicast(metaData, &startMessage);

    Message startReceiver;
    receiveMessages(metaData, &startReceiver);
    logReceiveStart(metaData->localId, payload);

    Message doneMessage;
    logDone(metaData->localId, payload);
    buildMessage(&doneMessage, payload, DONE);
    send_multicast(metaData, &doneMessage);

    Message doneReceiver;
    receiveMessages(metaData, &doneReceiver);
    logReceiveDone(metaData->localId, payload);

    closePipes(&metaData->pipesData, metaData->procCount, metaData->localId);
    exit(0);
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        wait(NULL);
    }
}

void buildMessage(Message *message, char *payload, MessageType type) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = type;
    message->s_header.s_local_time = time(NULL);
    message->s_header.s_payload_len = strlen(payload);
    strcpy(message->s_payload, payload);
}

void receiveMessages(MetaData *metaData, Message *message) {
    for (int i = 1; i < metaData->procCount; ++i) {
        if (i != metaData->localId) {
            receive(metaData, i, message);
            //printMessage(message, metaData->localId);
        }
    }
}
