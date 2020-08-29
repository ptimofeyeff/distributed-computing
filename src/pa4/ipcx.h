#ifndef LAB1_IPCX_H
#define LAB1_IPCX_H

#include "ipc.h"
#include "branch.h"
#include "workers.h"


void syncReceiveFromAllChild(void *, Message []);

void syncReceive(void *, local_id, Message*);

int sendToAllChild(void *, const Message *);

int sendToAllWorkers(BranchData *branchData, Message *message, Workers *workers);

void syncReceiveDoneFromAllWorkers(void *self, Message message[], Workers*);

int receiveFromAnyWorkers(void *self, Message *message);

#endif //LAB1_IPCX_H
