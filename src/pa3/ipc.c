#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "ipc.h"
#include "ipcx.h"
#include "lamport.h"

int send(void *self, local_id destination, const Message *message) {
    BranchData *branchData = (BranchData *) self;
    local_id from = branchData->id;
    local_id to = destination;

    size_t result = write(branchData->descriptors->descriptors[from][to][WRITE_DESC], message,
                          sizeof(message->s_header) + message->s_header.s_payload_len);
    if (result != -1) {
        //printf("in proc %d success send from %d to %d message type %d\n", branchData->id, from, to,  message->s_header.s_type);
        //fflush(stdout);
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

int receive(void *self, local_id sender, Message *message) {
    BranchData *branchData = (BranchData *) self;
    local_id from = sender;
    local_id to = branchData->id;
    //printf("in proc %d try to receive from %d to %d\n", branchData->id, from, to);
    //fflush(stdout);

    int header = read(branchData->descriptors->descriptors[from][to][READ_DESC], &message->s_header, sizeof (message->s_header));

    if (header > 0) {
        read(branchData->descriptors->descriptors[from][to][READ_DESC], &message->s_payload, message->s_header.s_payload_len);
        //printf("in proc %d success receive %d byte from %d to %d message type %d\n",
          //     branchData->id, body, from, to, message->s_header.s_type); // may 0 byte, why?!
        //fflush(stdout);
        return 0;
    } else {
        if (header == -1 && errno == EAGAIN) {
            //printf("fail to receive from %d to %d, no message found\n", from, to);
        } else {
            printf("fail to receive from %d to %d, error is %d\n", from, to, errno);
        }
        fflush(stdout);
        return -1;
    }
}

int receive_any(void *self, Message *message) {
    BranchData *branchData = (BranchData *) self;
    while (1) {
        for (int i = 0; i < branchData->branchCount; ++i) {
            if (i != branchData->id) {
                int result = receive(self, i, message);
                if (result == 0) {
                    if (branchData->logicTime < message->s_header.s_local_time) {
                        setLamportTime(message->s_header.s_local_time);
                    }
                    incrementLamportTime();
                    return 0;
                }
            }
        }
    }
}

// not implemented lamport increase
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

void syncReceive(void * self, local_id sender, Message *message) {
    BranchData *branchData = (BranchData *) self;
    while (1) {
        if (receive(self, sender, message) == 0) {
            if (branchData->logicTime < message->s_header.s_local_time) {
                setLamportTime(message->s_header.s_local_time);
            }
            incrementLamportTime();
            break;
        }
    }
}
