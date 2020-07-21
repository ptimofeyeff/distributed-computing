#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#include "pa1_starter_code/pa1.h"
#include "pa1_starter_code/ipc.h"
#include "pa1_starter_code/common.h"

void createProcesses(int count);

void run(local_id id);

void waitProcesses(int count);

void initPipes();

void finalizePipes();

void sayHello(char *, local_id);


int fd[2];
FILE *eventsLogs;
FILE *pipesLogs;

static const char *const log_open_pipe_descr_r =
        "pipe descriptor number %d has open for reading in process %1d (pid %5d, parent %5d)\n";
static const char *const log_open_pipe_descr_w =
        "pipe descriptor number %d has open for writing in process %1d (pid %5d, parent %5d)\n";
static const char *const log_close_pipe_descr =
        "pipe descriptor number %d has close in process %1d (pid %5d, parent %5d)\n";

int main(int argc, char *argv[]) {

    /**
     * 1. Осуществление необходимой подготовки для организации межпроцессорного взаимодействия
     * 2. Создание X идентичных дочерних процессов
     * 3. Мониторинг работы дочерних процессов
     */


    Message message;

    int processesCount = (int) strtol(argv[2], NULL, 10);

    eventsLogs = fopen(events_log, "w");

    initPipes();


    createProcesses(processesCount);
    waitProcesses(processesCount);

    for (int i = 0; i < processesCount; ++i) {
        read(fd[0], &message, MAX_MESSAGE_LEN);
        printf("%d\n", message.s_header.s_local_time);
    }

    finalizePipes();

    return 0;
}


void createProcesses(int count) {
    for (int i = 1; i < count + 1; ++i) {
        if (fork() == 0) {  // если это дочерний процесс, то ...
            run(i);
        }
    }
}

void run(local_id id) {

    char startedMessage[MAX_PAYLOAD_LEN];

    sayHello(startedMessage, id);

    Message message;

    message.s_header.s_magic = MESSAGE_MAGIC;
    message.s_header.s_type = STARTED;
    // тут записываем отрицательное число из-за 16-разраядной знаковой переменной для времени
    message.s_header.s_local_time = time(NULL);
    message.s_header.s_payload_len = strlen(startedMessage);

    strcpy(message.s_payload, startedMessage);

    write(fd[1], &message, MAX_MESSAGE_LEN);

    close(fd[0]);
    close(fd[1]);

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
    /*
     * TODO
     */
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
    /*
     * TODO
     */
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

void initPipes() {
    pipesLogs = fopen(pipes_log, "w");
    pipe(fd);
    fprintf(pipesLogs, log_open_pipe_descr_r, fd[0], 0, getpid(), getppid());
    fprintf(pipesLogs, log_open_pipe_descr_w, fd[1], 0, getpid(), getppid());
    fflush(pipesLogs);
    printf(log_open_pipe_descr_r, fd[0], 0, getpid(), getppid());
    printf(log_open_pipe_descr_w, fd[1], 0, getpid(), getppid());
}

void finalizePipes() {
    close(fd[0]);
    close(fd[1]);
    fprintf(pipesLogs, log_close_pipe_descr, fd[0], 0, getpid(), getppid());
    fprintf(pipesLogs, log_close_pipe_descr, fd[1], 0, getpid(), getppid());
    printf(log_close_pipe_descr, fd[0], 0, getpid(), getppid());
    printf(log_close_pipe_descr, fd[1], 0, getpid(), getppid());
    fclose(pipesLogs);
}

void sayHello(char *startedMessage, local_id id) {
    sprintf(startedMessage, log_started_fmt, id, getpid(), getppid());
    fprintf(eventsLogs, "%s", startedMessage);
    printf("%s", startedMessage);

}