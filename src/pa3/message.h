#ifndef LAB1_MESSAGE_H
#define LAB1_MESSAGE_H

#include "ipc.h"
#include "banking.h"
#include "branch.h"

void buildEmptyMessage(Message *, char *, MessageType);

void buildTransferMessage(Message *, TransferOrder *);

void buildAckMessage(Message *);

void buildStopMessage(Message *);

void buildHistoryMessage(Message *, BalanceHistory *);

#endif //LAB1_MESSAGE_H
