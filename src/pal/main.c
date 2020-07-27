#include "main.h"

FILE *eventsLogs;
FILE *pipesLogs;
int const DESC_COUNT = 2;
int const READ_DESC = 0;
int const WRITE_DESC = 1;


void createChild(int procCount, int [procCount][procCount][DESC_COUNT]);

void run(local_id id, int procCount, int [procCount][procCount][DESC_COUNT]);

void waitChild(int count);

void initPipes(int procCount, int [procCount][procCount][DESC_COUNT]);

void finalizePipes(int procCount, int [procCount][procCount][DESC_COUNT]);

void logStarted(local_id, char *);

void logDone (local_id, char *);


struct MetaData {
    local_id selfID;
    int procCount;
    int ***pipes;
};
typedef struct MetaData MetaData;


static const char *const log_open_pipe_descr_r =
        "pipe descriptor number %d has open for reading in process %1d (pid %5d, parent %5d)\n";
static const char *const log_open_pipe_descr_w =
        "pipe descriptor number %d has open for writing in process %1d (pid %5d, parent %5d)\n";
static const char *const log_close_pipe_descr =
        "pipe descriptor number %d has close in process %1d (pid %5d, parent %5d)\n";

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    int procCount = cpCount + 1;
    int pipes[procCount][procCount][DESC_COUNT];
    initPipes(procCount, pipes);

    createChild(procCount, pipes);
    waitChild(cpCount);

    MetaData metaData;
    metaData.pipes = (int ***) &pipes;
    metaData.procCount = procCount;
    metaData.selfID = PARENT_ID;

    Message message;


    /*for (int i = 1; i < procCount; ++i) {
        receive(&metaData, i, &message);
        printf("parent process receive:\n");
        printf("message magic = %d\n", message.s_header.s_magic);
        printf("message local time = %d\n", message.s_header.s_local_time);
        printf("message payload length = %d\n", message.s_header.s_payload_len);
        printf("message type = %d\n", message.s_header.s_type);
        printf("message payload = %s\n\n", message.s_payload);
    }*/

    finalizePipes(procCount, pipes);

    return 0;
}


void createChild(int procCount, int pipes[procCount][procCount][DESC_COUNT]) {
    for (int i = 1; i < procCount; ++i) {
        if (fork() == 0) {
            run(i, procCount, pipes);
        }
    }
}

void waitChild(int count) {
    for (int i = 0; i < count; i++) {
        int pid = wait(NULL);
        //printf("child process with pid = %d was finished\n", pid);
    }
}

void run(local_id id, int procCount, int pipes[procCount][procCount][DESC_COUNT]) {

    char payload[MAX_PAYLOAD_LEN];
    logStarted(id, payload);
    Message message;
    message.s_header.s_magic = MESSAGE_MAGIC;
    message.s_header.s_type = STARTED;
    message.s_header.s_local_time = time(NULL);
    message.s_header.s_payload_len = strlen(payload);
    strcpy(message.s_payload, payload);

    MetaData metaData;
    metaData.pipes = (int ***) &pipes;
    metaData.procCount = procCount;
    metaData.selfID = id;

    send(&metaData, PARENT_ID, &message);


    logDone(id, payload);

    message.s_header.s_type = DONE;
    message.s_header.s_local_time = time(NULL);
    message.s_header.s_payload_len = strlen(payload);
    strcpy(message.s_payload, payload);

    send(&metaData, PARENT_ID, &message);

    // тут можно попробовать позакрывать дескрипторов

    exit(0);
}



// Отправляет сообщение процессу по его айди, в случаи успеха вернет 0
int send(void *self, local_id destination, const Message *message) {
    MetaData *metaData = (MetaData *) self;
    local_id source = metaData->selfID;
    int ***pipes = metaData->pipes;
    write(pipes[source][destination][WRITE_DESC], &message, message->s_header.s_payload_len + sizeof(message->s_header));
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
// Похоже аллоцировать под сообщение придется самому
int receive(void *self, local_id source, Message *message) {
    MetaData *metaData = (MetaData *) self;
    local_id destId = metaData->selfID;
    int ***pipes = metaData->pipes;
    read(pipes[source][destId][READ_DESC], &message, MAX_MESSAGE_LEN);
    return 0;
}

// Получает сообщение от любого процесса
// Нужно самому выделять память под сообщение
int receive_any(void *self, Message *message) {
    /*
     * TODO
     */
    return 0;
}

void initPipes(int procCount, int pipes[procCount][procCount][DESC_COUNT]) {
    pipesLogs = fopen(pipes_log, "w");
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                pipe(pipes[i][j]);
                fprintf(pipesLogs, log_open_pipe_descr_r, pipes[i][j][READ_DESC], PARENT_ID, getpid(), getppid());
                fprintf(pipesLogs, log_open_pipe_descr_w, pipes[i][j][WRITE_DESC], PARENT_ID, getpid(), getppid());
                fflush(pipesLogs);
                printf(log_open_pipe_descr_r, pipes[i][j][READ_DESC], PARENT_ID, getpid(), getppid());
                printf(log_open_pipe_descr_w, pipes[i][j][WRITE_DESC], PARENT_ID, getpid(), getppid());
            }
        }

    }
}

void finalizePipes(int procCount, int pipes[procCount][procCount][DESC_COUNT]) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                close(pipes[i][j][READ_DESC]);
                close(pipes[i][j][WRITE_DESC]);
                fprintf(pipesLogs, log_close_pipe_descr, pipes[i][j][READ_DESC], 0, getpid(), getppid());
                fprintf(pipesLogs, log_close_pipe_descr, pipes[i][j][WRITE_DESC], 0, getpid(), getppid());
                printf(log_close_pipe_descr, pipes[i][j][READ_DESC], 0, getpid(), getppid());
                printf(log_close_pipe_descr, pipes[i][j][WRITE_DESC], 0, getpid(), getppid());
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