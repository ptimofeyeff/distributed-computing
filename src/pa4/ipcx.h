#ifndef LAB1_IPCX_H
#define LAB1_IPCX_H

#include "ipc.h"
#include "branch.h"


void syncReceiveFromAllChild(void *, Message []);

void syncReceive(void *, local_id, Message*);

int sendToAllChild(void *, const Message *);

#endif //LAB1_IPCX_H
