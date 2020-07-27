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
    local_id localId;
} MetaData;


void createChild(int procCount, MetaData *);

void run(local_id id, int procCount, MetaData *);

void waitChild(int count);

void initPipes(int procCount, MetaData *);

void finalizePipes(int procCount, MetaData *);

void logStarted(local_id, char *);

void logDone(local_id, char *);

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    int procCount = cpCount + 1;

    MetaData metaData;

    metaData.pipesData.procCount = procCount; // длинна и ширина матрицы пайпов
    metaData.localId = PARENT_ID;

    initPipes(procCount, &metaData);

    createChild(procCount, &metaData);
    waitChild(cpCount);


    Message message;


    for (int i = 0; i < 2; ++i) {
        receive(&metaData, i, &message);
        printf("parent process receive:\n");
        printf("message magic = %d\n", message.s_header.s_magic);
        printf("message local time = %d\n", message.s_header.s_local_time);
        printf("message payload length = %d\n", message.s_header.s_payload_len);
        printf("message type = %d\n", message.s_header.s_type);
        printf("message payload = %s\n\n", message.s_payload);
    }

    finalizePipes(procCount, &metaData);

    return 0;
}


void createChild(int procCount, MetaData *metaData) {
    for (int i = 1; i < procCount; ++i) {
        if (fork() == 0) {  // прокидываем одну общую методату с матрицей пайпов в каждого ребенка
            run(i, procCount, metaData);
        }
    }
}

void waitChild(int cpCount) {
    for (int i = 0; i < cpCount; i++) {
        int pid = wait(NULL);
        //printf("child process with pid = %d was finished\n", pid);
    }
}

void run(local_id id, int procCount, MetaData *metaData) {

    char payload[MAX_PAYLOAD_LEN];
    logStarted(id, payload);
    Message message;
    message.s_header.s_magic = MESSAGE_MAGIC;
    message.s_header.s_type = STARTED;
    message.s_header.s_local_time = time(NULL);
    message.s_header.s_payload_len = strlen(payload);
    strcpy(message.s_payload, payload);

    //metaData->localId = (local_id) malloc(sizeof(local_id));
    //metaData->localId = id;
    send(metaData, PARENT_ID, &message);


    logDone(id, payload);

    message.s_header.s_type = DONE;
    message.s_header.s_local_time = time(NULL);
    message.s_header.s_payload_len = strlen(payload);
    strcpy(message.s_payload, payload);

    send(metaData, PARENT_ID, &message);


    // здесь можно попробовать позакрывать дескрипторы

    exit(0);
}


// Отправляет сообщение процессу по его айди, в случаи успеха вернет 0
int send(void *self, local_id destination, const Message *message) {
    MetaData *metaData = (MetaData *) self;
    write(metaData->pipesData.pipes[1][0][WRITE_DESC], message, sizeof *message);
    return 0;
}

// Посылает соообщение всем другим процессам, включая родителя
// должен останавливаться на первой ошибке
int send_multicast(void *self, const Message *message) {
    /*
     * TODO
     */

    return 0;
}


// Получает сообщение от процесса по его айди
int receive(void *self, local_id source, Message *message) {
    MetaData *metaData = (MetaData *) self;
    read(metaData->pipesData.pipes[1][0][READ_DESC], message, sizeof *message);
    return 0;
}

// Получает сообщение от любого процесса
int receive_any(void *self, Message *message) {
    /*
     * TODO
     */
    return 0;
}

void initPipes(int procCount, MetaData *metaData) {
    pipesLogs = fopen(pipes_log, "w");
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
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

void finalizePipes(int procCount, MetaData *metaData) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                close(metaData->pipesData.pipes[i][j][READ_DESC]);
                close(metaData->pipesData.pipes[i][j][WRITE_DESC]);
                fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][j][READ_DESC], 0, getpid(),
                        getppid());
                fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][j][WRITE_DESC], 0, getpid(),
                        getppid());
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