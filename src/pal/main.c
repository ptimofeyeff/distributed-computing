#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#include "pa1_starter_code/pa1.h"
#include "pa1_starter_code/ipc.h"

void createProcesses(int count);

void run(local_id id);

void waitProcesses(int count);

int fd[2];


int main(int argc, char *argv[]) {

    /**
     * 1. Осуществление необходимой подготовки для организации межпроцессорного взаимодействия
     * 2. Создание X идентичных дочерних процессов
     * 3. Мониторинг работы дочерних процессов
     */


    Message message;
    pipe(fd);

    int processesCount = (int) strtol(argv[2], NULL, 10);

    createProcesses(processesCount);
    waitProcesses(processesCount);


    for (int i = 0; i <processesCount; ++i) {
        read(fd[0], &message, MAX_MESSAGE_LEN);
        printf("%d\n", message.s_header.s_payload_len);
    }

    close(fd[0]);
    close(fd[1]);

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

    char createLog[MAX_PAYLOAD_LEN];

    sprintf(createLog, log_started_fmt, id, getpid(), getppid());
    printf("%s", createLog);

    Message message;

    message.s_header.s_magic = MESSAGE_MAGIC;
    message.s_header.s_type = STARTED;
    message.s_header.s_local_time = time(NULL);
    message.s_header.s_payload_len = strlen(createLog);

    strcpy(message.s_payload, createLog);

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
