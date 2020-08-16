#ifndef LAB1_MESSAGE_H
#define LAB1_MESSAGE_H

#include "branch.h"

void buildEmptyMessage(Message *, char *, MessageType);

void buildTransferMessage(Message *, TransferOrder *);

void buildAckMessage(Message *);

void buildStopMessage(Message *);

void receiveFromAll(BranchData *, Message *);

void buildHistoryMessage(Message *, BalanceHistory *);

#endif //LAB1_MESSAGE_H
