#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "ipc.h"
#include "ipcx.h"
#include "lamport.h"
#include "workers.h"

int send(void *self, local_id destination, const Message *message) {
    BranchData *branchData = (BranchData *) self;
    local_id from = branchData->id;
    local_id to = destination;

    size_t result = write(branchData->descriptors->descriptors[from][to][WRITE_DESC], message,
                          sizeof(message->s_header) + message->s_header.s_payload_len);
    if (result != -1) {
        return 0;
    } else {
        printf("fail to send from %d to %d\n", from, to);
        return -1;
    }
}

int send_multicast(void *self, const Message *message) {
    BranchData *branchData = (BranchData *) self;
    for (int i = 0; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            int result = send(self, i, message);
            if (result == -1) {
                printf("fail to send_multicast\n");
                return -1;
            }
        }
    }
    return 0;
}

int sendToAllChild(void *self, const Message *message) {
    BranchData *branchData = (BranchData *) self;
    for (int i = 1; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            int result = send(self, i, message);
            if (result == -1) {
                printf("fail to send_multicast child\n");
                return -1;
            }
        }
    }
    return 0;
}

int sendToAllWorkers(BranchData *branchData, Message *message, Workers *workers) {
    for (int i = 0; i < workers->length; ++i) {
        if (workers->procId[i] != branchData->id) {
            int result = send(branchData, workers->procId[i], message);
            if (result == -1) {
                printf("fail to send to all workers child\n");
                return -1;
            }
        }
    }
    return 0;
}


int receive(void *self, local_id sender, Message *message) {
    BranchData *branchData = (BranchData *) self;
    local_id from = sender;
    local_id to = branchData->id;

    int header = read(branchData->descriptors->descriptors[from][to][READ_DESC], &message->s_header, sizeof (message->s_header));

    if (header > 0) {
        read(branchData->descriptors->descriptors[from][to][READ_DESC], &message->s_payload, message->s_header.s_payload_len);
        branchData->senderId = sender;
        if (branchData->logicTime < message->s_header.s_local_time) {
            setLamportTime(message->s_header.s_local_time);
        }
        incrementLamportTime();
        return 0;
    } else {
        if (header == -1 && errno == EAGAIN) {
        } else {
            printf("fail to receive from %d to %d, error is %d\n", from, to, errno);
            sleep(1);
        }
        fflush(stdout);
        return -1;
    }
}

int receive_any(void *self, Message *message) {
    BranchData *branchData = (BranchData *) self;
    for (int i = 0; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            int result = receive(self, i, message);
            if (result == 0) {
                return 0;
            }
        }
    }
    return -1;
}

void syncReceiveFromAllChild(void *self, Message message[]) {
    BranchData *branchData = (BranchData *) self;
    for (int i = 1; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            while (1) {
                if (receive(branchData, i, &message[i]) == 0) {
                    break;
                }
            }
        }
    }
}

void syncReceiveDoneFromAllWorkers(void *self, Message message[], Workers *workers) {
    BranchData *branchData = (BranchData *) self;
    for (int i = 0; i < workers->length; ++i) {
        if (workers->procId[i] != branchData->id) {
            while (1) {
                if (receive(branchData, workers->procId[i], &message[i]) == 0) {
                    if (message[i].s_header.s_type == DONE) {
                        break;
                    }
                }
            }
        }
    }
}

void syncReceive(void * self, local_id sender, Message *message) {
    while (1) {
        if (receive(self, sender, message) == 0) {
            break;
        }
    }
}
