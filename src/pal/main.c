#include "main.h"

#define DESC_COUNT 2
#define READ_DESC 0
#define WRITE_DESC 1
#define MAX_PROC 11

FILE *eventsLogs;
FILE *pipesLogs;

typedef struct {
    int procCount;
    int pipes[MAX_PROC][MAX_PROC][DESC_COUNT];
} PipesData;

typedef struct {
    PipesData pipesData;
    local_id *localId;
} MetaData;


void createChild(MetaData *);

void run(local_id id, MetaData *);

void waitChild(int count);

void initPipes(MetaData *);

void finalizePipes(MetaData *);

void logStarted(local_id, char *);

void logDone(local_id, char *);

void logReceiveStart(local_id, char *);

void logReceiveDone(local_id, char *);

void printMessage(Message *message, local_id id);

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    int procCount = cpCount + 1;

    MetaData metaData;
    metaData.pipesData.procCount = procCount; // длинна и ширина матрицы пайпов

    initPipes(&metaData);

    createChild(&metaData);
    //waitChild(cpCount);

    Message message;

    local_id parentId = PARENT_ID;
    metaData.localId = &parentId;

    // receive started
    for (int i = 1; i <= cpCount; ++i) {
        receive(&metaData, i, &message);
    }

    // receive done
    for (int i = 1; i <= cpCount; ++i) {
        receive(&metaData, i, &message);
    }

    finalizePipes(&metaData);

    return 0;
}


void createChild(MetaData *metaData) {
    for (int i = 1; i < metaData->pipesData.procCount; ++i) {
        if (fork() == 0) {  // прокидываем одну общую методату с матрицей пайпов в каждого ребенка
            run(i, metaData);
        }
    }
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        int pid = wait(NULL);
        //printf("child process with pid = %d was finished\n", pid);
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

    Message startReceivers[metaData->pipesData.procCount - 1];

    for (int i = 1; i <metaData->pipesData.procCount; ++i) {
        if (i != *metaData->localId) {
            receive(metaData, i, &startReceivers[i]);
            //printMessage(&startReceivers[i], *metaData->localId);
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

    Message doneReceiver[metaData->pipesData.procCount - 1];

    for (int i = 1; i <metaData->pipesData.procCount; ++i) {
        if (i != *metaData->localId) {
            receive(metaData, i, &doneReceiver[i]);
            printMessage(&doneReceiver[i], *metaData->localId);
        }
    }

    logReceiveDone(id, payload);

    exit(0);
}


// Отправляет сообщение процессу по его айди, в случаи успеха вернет 0
int send(void *self, local_id destination, const Message *message) {
    MetaData *metaData = (MetaData *) self;
    local_id from = *metaData->localId;
    local_id to = destination;
    printf("in proc %d try to send from %d to %d\n", *metaData->localId, from, to);
    size_t result = write(metaData->pipesData.pipes[from][to][WRITE_DESC], message, sizeof *message);
    if (result != EXIT_FAILURE) { // по значению результата можно смотерть сколько байтов переслалось
        printf("in proc %d success send from %d to %d\n", *metaData->localId, from, to);
        return EXIT_SUCCESS;
    } else {
        printf("fail to send from %d to %d\n", from, to);
        return EXIT_FAILURE;
    }
}

// Посылает соообщение всем другим процессам, включая родителя
// должен останавливаться на первой ошибке
int send_multicast(void *self, const Message *message) {
    MetaData *metaData = (MetaData *) self;
    for (int i = 0; i < metaData->pipesData.procCount; ++i) {
        if (i != *metaData->localId) {
            int result = send(self, i, message);
            if (result == EXIT_FAILURE) {
                printf("fail to send_multicast\n");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}


// Получает сообщение от процесса по его айди
int receive(void *self, local_id sender, Message *message) {

    MetaData *metaData = (MetaData *) self;
    local_id from = sender;
    local_id to = *metaData->localId;
    printf("in proc %d try to receive from %d to %d\n", *metaData->localId, from, to);

    int result = read(metaData->pipesData.pipes[from][to][READ_DESC], message, sizeof *message);

    if (result != EXIT_FAILURE) {
        if (*metaData->localId == 10) {
            printf("\nhello\n");
        }
        printf("in proc %d success receive from %d to %d\n", *metaData->localId, from, to );
        return EXIT_SUCCESS;
    } else {
        printf("fail to receive from %d to %d\n", from, to);
        return EXIT_FAILURE;
    }
}

// Получает сообщение от любого процесса
int receive_any(void *self, Message *message) {

    MetaData *metaData = (MetaData *) self;
    for (int i = 0; i < metaData->pipesData.procCount; ++i) {
        if (i != *metaData->localId) {
            int result = receive(self, i, message);
            if (result == EXIT_FAILURE) {
                printf("fail to send_multicast\n");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}

void initPipes(MetaData *metaData) {
    pipesLogs = fopen(pipes_log, "w");
    for (int i = 0; i < metaData->pipesData.procCount; ++i) {
        for (int j = 0; j < metaData->pipesData.procCount; ++j) {
            if (i != j) {
                pipe(metaData->pipesData.pipes[i][j]);
                fprintf(pipesLogs, log_open_pipe_descr_r, metaData->pipesData.pipes[i][j][READ_DESC], PARENT_ID, getpid(), getppid());
                fprintf(pipesLogs, log_open_pipe_descr_w, metaData->pipesData.pipes[i][j][WRITE_DESC], PARENT_ID,getpid(), getppid());
                fflush(pipesLogs);
                printf(log_open_pipe_descr_r, metaData->pipesData.pipes[i][j][READ_DESC], PARENT_ID, getpid(), getppid());
                printf(log_open_pipe_descr_w, metaData->pipesData.pipes[i][j][WRITE_DESC], PARENT_ID, getpid(), getppid());
            }
        }

    }
}

void finalizePipes (MetaData *metaData) {
    for (int i = 0; i < metaData->pipesData.procCount; ++i) {
        for (int j = 0; j < metaData->pipesData.procCount; ++j) {
            if (i != j) {
                close(metaData->pipesData.pipes[i][j][READ_DESC]);
                close(metaData->pipesData.pipes[i][j][WRITE_DESC]);
                fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][j][READ_DESC], 0, getpid(), getppid());
                fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][j][WRITE_DESC], 0, getpid(),getppid());
                printf(log_close_pipe_descr, metaData->pipesData.pipes[i][j][READ_DESC], 0, getpid(), getppid());
                printf(log_close_pipe_descr, metaData->pipesData.pipes[i][j][WRITE_DESC], 0, getpid(), getppid());
            }
        }
    }
    fclose(pipesLogs);
}

void logStarted(local_id id, char *startedMessage) {
    sprintf(startedMessage, log_started_fmt, id, getpid(), getppid());
    fprintf(eventsLogs, "%s", startedMessage);
    fflush(eventsLogs);
    printf("%s", startedMessage);
}

void logDone(local_id id, char *doneMessage) {
    sprintf(doneMessage, log_done_fmt, id);
    fprintf(eventsLogs, "%s", doneMessage);
    fflush(eventsLogs);
    printf("%s", doneMessage);
}

void logReceiveStart(local_id id, char *receiveStartMsg) {
    sprintf(receiveStartMsg, log_received_all_started_fmt, id);
    fprintf(eventsLogs, "%s", receiveStartMsg);
    fflush(eventsLogs);
    printf("%s", receiveStartMsg);
}

void logReceiveDone(local_id id, char *receiveDoneMsg) {
    sprintf(receiveDoneMsg, log_received_all_done_fmt, id);
    fprintf(eventsLogs, "%s", receiveDoneMsg);
    fflush(eventsLogs);
    printf("%s", receiveDoneMsg);
}


void printMessage(Message *message, local_id id) {
    printf("\n\nprocess with id %d receive:\n"
           "message magic = %d\n"
           "message local time = %d\n"
           "message payload length = %d\n"
           "message type = %d\n"
           "message payload = %s\n\n",
           id, message->s_header.s_magic, message->s_header.s_local_time,  message->s_header.s_payload_len,
           message->s_header.s_type, message->s_payload);
}