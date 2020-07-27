#include "main.h"

FILE *eventsLogs;
FILE *pipesLogs;
int const DESC_COUNT = 2;
int const READ_DESC = 0;
int const WRITE_DESC = 1;


void createProcesses(int count, int [][DESC_COUNT], int);

void run(local_id id, int [][DESC_COUNT], int);

void waitProcesses(int count);

void initPipes(int [][DESC_COUNT], int);

void finalizePipes(int [][DESC_COUNT], int);

void logStarted(char *, local_id);

void logDone(char *, local_id);

int computePipesCount(int);

struct Self {
    local_id sourceId;
    int pipesCount;
    int **pipes;
};
typedef struct Self Self;

struct Pipe {
    int pipeCount;
    int pipeC[][pipeCount];
    int pipes[];
};



static const char *const log_open_pipe_descr_r =
        "pipe descriptor number %d has open for reading in process %1d (pid %5d, parent %5d)\n";
static const char *const log_open_pipe_descr_w =
        "pipe descriptor number %d has open for writing in process %1d (pid %5d, parent %5d)\n";
static const char *const log_close_pipe_descr =
        "pipe descriptor number %d has close in process %1d (pid %5d, parent %5d)\n";

int main(int argc, char *argv[]) {

    eventsLogs = fopen(events_log, "w");

    int cpCount = (int) strtol(argv[2], NULL, 10);
    int pipeCount = computePipesCount(cpCount);

    int pipes[cpCount + 1][cpCount + 1][DESC_COUNT];

    initPipes(pipes, pipeCount);

    createProcesses(processesCount, pipes, pipeCount);
    waitProcesses(processesCount);

    Self self;
    self.pipes = (int **) &pipes;
    self.pipesCount = pipeCount;
    self.sourceId = PARENT_ID;

    Message message;


    for (int i = 1; i < processesCount; ++i) {
        receive(&self, 1, &message);
        printf("parent process receive:\n");
        printf("message magic = %d\n", message.s_header.s_magic);
        printf("message local time = %d\n", message.s_header.s_local_time);
        printf("message payload length = %d\n", message.s_header.s_payload_len);
        printf("message type = %d\n", message.s_header.s_type);
        printf("message payload = %s\n\n", message.s_payload);
    }

    finalizePipes(pipes, pipeCount);

    return 0;
}


void createProcesses(int processesCount, int pipes[][DESC_COUNT], int pipesCount) {
    for (int i = 1; i < processesCount + 1; ++i) {
        if (fork() == 0) {
            int processPipes[processesCount * 2][DESC_COUNT];

            for (int j = 0; j < processesCount * 2; ++j) {
                processPipes[j][READ_DESC] = pipes[j + (i * processesCount)][READ_DESC];
                processPipes[j][WRITE_DESC] = pipes[j + (i * processesCount)][WRITE_DESC];
            }

            run(i, pipes, pipesCount);
        }
    }
}

void run(local_id id, int pipes[][DESC_COUNT], int pipesCount) {

    char payload[MAX_PAYLOAD_LEN];

    logStarted(payload, id);

    Message message;
    message.s_header.s_magic = MESSAGE_MAGIC;
    message.s_header.s_type = STARTED;
    message.s_header.s_local_time = time(NULL);
    message.s_header.s_payload_len = strlen(payload);
    strcpy(message.s_payload, payload);

    Self self;
    self.pipes = (int **) &pipes;
    self.pipesCount = pipesCount;
    self.sourceId = id;

    send(&self, PARENT_ID, &message);


    logDone(payload, id);

    message.s_header.s_type = DONE;
    message.s_header.s_local_time = time(NULL);
    message.s_header.s_payload_len = strlen(payload);
    strcpy(message.s_payload, payload);

    send(&self, PARENT_ID, &message);


    // Почему недостаточно finalizePipes()?
    close(pipes[id][READ_DESC]);
    close(pipes[id][WRITE_DESC]);

    exit(0);
}

void waitProcesses(int count) {
    for (int i = 0; i < count; i++) {
        int pid = wait(NULL);
        //printf("child process with pid = %d was finished\n", pid);
    }
}


// Отправляет сообщение процессу по его айди, в случаи успеха вернет 0
int send(void *self, local_id destination, const Message *message) {
    Self *metaData = (Self *) self;

    local_id source = metaData->sourceId;
    int **pipes = metaData->pipes;
    int pipeCount = metaData->pipesCount;

    write(pipes[source][WRITE_DESC], &message, message->s_header.s_payload_len + sizeof(message->s_header));

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
    Self *metaData = (Self *) self;
    int **pipes = metaData->pipes;


    read(pipes[source][READ_DESC], &message, MAX_MESSAGE_LEN);
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

void initPipes(int pipes[][DESC_COUNT], int pipesCount) {
    pipesLogs = fopen(pipes_log, "w");
    for (int i = 0; i < pipesCount; ++i) {
        pipe(pipes[i]);
        fprintf(pipesLogs, log_open_pipe_descr_r, pipes[i][READ_DESC], 0, getpid(), getppid());
        fprintf(pipesLogs, log_open_pipe_descr_w, pipes[i][WRITE_DESC], 0, getpid(), getppid());
        fflush(pipesLogs);
        printf(log_open_pipe_descr_r, pipes[i][READ_DESC], 0, getpid(), getppid());
        printf(log_open_pipe_descr_w, pipes[i][WRITE_DESC], 0, getpid(), getppid());
    }
}

void finalizePipes(int pipes[][DESC_COUNT], int pipesCount) {
    for (int i = 0; i < pipesCount; ++i) {
        close(pipes[i][0]);
        close(pipes[i][1]);
        fprintf(pipesLogs, log_close_pipe_descr, pipes[i][READ_DESC], 0, getpid(), getppid());
        fprintf(pipesLogs, log_close_pipe_descr, pipes[i][WRITE_DESC], 0, getpid(), getppid());
        printf(log_close_pipe_descr, pipes[i][READ_DESC], 0, getpid(), getppid());
        printf(log_close_pipe_descr, pipes[i][WRITE_DESC], 0, getpid(), getppid());
    }
    fclose(pipesLogs);
}

void logStarted(char *startedMessage, local_id id) {
    sprintf(startedMessage, log_started_fmt, id, getpid(), getppid());
    fprintf(eventsLogs, "%s", startedMessage);
    printf("%s", startedMessage);
}

void logDone(char *doneMessage, local_id id) {
    sprintf(doneMessage, log_done_fmt, id);
    fprintf(eventsLogs, "%s", doneMessage);
    printf("%s", doneMessage);
}

int computePipesCount(int processes) {
    if (processes == 1) {
        return 2;
    } else {
        return computePipesCount(processes - 1) + 2 * processes;
    }
}