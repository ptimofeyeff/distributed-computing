#ifndef LAB1_MESSAGEUTILS_H
#define LAB1_MESSAGEUTILS_H

#include "branch.h"

void buildEmptyMessage(Message *, char *, MessageType);

void buildTransferMessage(Message *, TransferOrder *);

void receiveFromAll(BranchData *, Message *);

#endif //LAB1_MESSAGEUTILS_H
