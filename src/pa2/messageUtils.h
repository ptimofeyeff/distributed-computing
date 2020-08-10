#ifndef LAB1_MESSAGEUTILS_H
#define LAB1_MESSAGEUTILS_H

#include "ipc.h"
#include "banking.h"

void buildEmptyMessage(Message *, char *, MessageType);

void buildTransferMessage(Message *, TransferOrder *);

#endif //LAB1_MESSAGEUTILS_H
