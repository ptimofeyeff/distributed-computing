#include "main.h"
#include "logs.h"
#include "pipes.h"


void createChild(MetaData *);

void run(local_id id, MetaData *);

void waitChild(int count);

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    int procCount = cpCount + 1;

    MetaData metaData;
    metaData.pipesData.procCount = procCount; // длинна и ширина матрицы пайпов
    local_id parentId = PARENT_ID;
    metaData.localId = &parentId;

    initPipes(&metaData);

    createChild(&metaData);

    Message message;

    // receive started
    for (int i = 1; i <= cpCount; ++i) {
        receive(&metaData, i, &message);
    }

    // receive done
    for (int i = 1; i <= cpCount; ++i) {
        receive(&metaData, i, &message);
    }

    finalizePipes(&metaData);

    waitChild(cpCount);
    return 0;
}


void createChild(MetaData *metaData) {
    for (int i = 1; i < metaData->pipesData.procCount; ++i) {
        if (fork() == 0) {  // прокидываем одну общую методату с матрицей пайпов в каждого ребенка
            run(i, metaData);
        }
    }
}


void run(local_id id, MetaData *metaData) {

    char payload[MAX_PAYLOAD_LEN];
    logStarted(id, payload);

    Message startSender;
    startSender.s_header.s_magic = MESSAGE_MAGIC;
    startSender.s_header.s_type = STARTED;
    startSender.s_header.s_local_time = time(NULL);
    startSender.s_header.s_payload_len = strlen(payload);
    strcpy(startSender.s_payload, payload);

    local_id localId = id;
    metaData->localId = &localId;

    send_multicast(metaData, &startSender);

    Message startReceivers;

    for (int i = 1; i < metaData->pipesData.procCount; ++i) {
        if (i != *metaData->localId) {
            receive(metaData, i, &startReceivers);
        }
    }

    logReceiveStart(id, payload);

    Message doneSender;
    logDone(id, payload);

    doneSender.s_header.s_type = DONE;
    doneSender.s_header.s_local_time = time(NULL);
    doneSender.s_header.s_payload_len = strlen(payload);
    strcpy(doneSender.s_payload, payload);

    send_multicast(metaData, &doneSender);

    Message doneReceiver;

    for (int i = 1; i < metaData->pipesData.procCount; ++i) {
        if (i != *metaData->localId) {
            receive(metaData, i, &doneReceiver);
        }
    }

    logReceiveDone(id, payload);

    exit(0);
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        wait(NULL);
    }
}
