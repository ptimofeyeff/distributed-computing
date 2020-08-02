#include "pa1_starter_code/ipc.h"
#include "main.h"
#include "stdio.h"
#include <unistd.h>


// Отправляет сообщение процессу по его айди, в случаи успеха вернет 0
int send(void *self, local_id destination, const Message *message) {
    MetaData *metaData = (MetaData *) self;
    local_id from = metaData->localId;
    local_id to = destination;
    printf("in proc %d try to send from %d to %d\n", metaData->localId, from, to);
    size_t result = write(metaData->pipesData.pipes[from][to][WRITE_DESC], message, sizeof *message);
    if (result != EXIT_FAILURE) { // по значению результата можно смотерть сколько байтов переслалось
        printf("in proc %d success send from %d to %d\n", metaData->localId, from, to);
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
        if (i != metaData->localId) {
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
    local_id to = metaData->localId;
    printf("in proc %d try to receive from %d to %d\n", metaData->localId, from, to);

    int result = read(metaData->pipesData.pipes[from][to][READ_DESC], message, sizeof *message);

    if (result != EXIT_FAILURE) {
        printf("in proc %d success receive from %d to %d\n", metaData->localId, from, to);
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
        if (i != metaData->localId) {
            int result = receive(self, i, message);
            if (result == EXIT_FAILURE) {
                printf("fail to send_multicast\n");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}
