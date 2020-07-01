#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "pa1_starter_code/pa1.h"
#include "pa1_starter_code/ipc.h"

void createProcesses(int count);
void run(local_id id);
void waitProcesses(int count);


int main(int argc, char *argv[]) {

    /**
     * 1. Осуществление необходимой подготовки для организации межпроцессорного взаимодействия
     * 2. Создание X идентичных дочерних процессов
     * 3. Мониторинг работы дочерних процессов
     */

    int processesCount = (int) strtol(argv[2], NULL, 10);

    createProcesses(processesCount);
    waitProcesses(processesCount);


    return 0;
}


void createProcesses(int count) {
    for (int i = 1; i < count + 1; ++i) {
        if (fork() == 0) {
            run(i);
        }
    }
}

void run(local_id id) {
    printf(log_started_fmt, id, getpid(), getppid());
    exit(0);
}

void waitProcesses(int count) {
    for (int i = 0; i < count; i++) {
        int pid = wait(NULL);
        printf("child process with pid = %d was finished\n", pid);
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
