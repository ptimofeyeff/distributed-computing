#include "ipc.h"
#include "pa23.h"
#include "stdio.h"
#include <unistd.h>

int send(void *self, local_id destination, const Message *message) {
    BranchData *branchData = (BranchData *) self;
    local_id from = branchData->id;
    local_id to = destination;
    printf("in proc %d try to send from %d to %d by descriptor = %d\n",
           branchData->id, from, to, branchData->descriptors.descriptors[from][to][WRITE_DESC]);
    fflush(stdout);
    size_t result = write(branchData->descriptors.descriptors[from][to][WRITE_DESC], message, sizeof *message);
    if (result != EXIT_FAILURE) { // по значению результата можно смотерть сколько байтов переслалось
        printf("in proc %d success send from %d to %d by descriptor = %d\n",
               branchData->id, from, to, branchData->descriptors.descriptors[from][to][WRITE_DESC]);
        fflush(stdout);
        return EXIT_SUCCESS;
    } else {
        printf("fail to send from %d to %d\n", from, to);
        return EXIT_FAILURE;
    }
}

int send_multicast(void *self, const Message *message) {
    BranchData *branchData = (BranchData *) self;
    for (int i = 0; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            int result = send(self, i, message);
            if (result == EXIT_FAILURE) {
                printf("fail to send_multicast\n");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}

int receive(void *self, local_id sender, Message *message) {
    BranchData *branchData = (BranchData *) self;
    local_id from = sender;
    local_id to = branchData->id;
    printf("in proc %d try to receive from %d to %d by descriptor = %d\n",
           branchData->id, from, to, branchData->descriptors.descriptors[from][to][READ_DESC]);
    fflush(stdout);
    int result = read(branchData->descriptors.descriptors[from][to][READ_DESC], message, sizeof *message);

    if (result != EXIT_FAILURE) {
        printf("in proc %d success receive from %d to %d by descriptor = %d\n",
               branchData->id, from, to, branchData->descriptors.descriptors[from][to][READ_DESC]);
        fflush(stdout);
        return EXIT_SUCCESS;
    } else {
        printf("fail to receive from %d to %d\n", from, to);
        return EXIT_FAILURE;
    }
}

int receive_any(void *self, Message *message) {
    BranchData *branchData = (BranchData *) self;
    for (int i = 0; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            int result = receive(self, i, message);
            if (result == EXIT_FAILURE) {
                printf("fail to send_multicast\n");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}
