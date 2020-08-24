#ifndef LAB1_MESSAGE_H
#define LAB1_MESSAGE_H

#include "ipc.h"
#include "banking.h"
#include "branch.h"

void buildStartMessage(Message *message, local_id id, balance_t balance);

void buildDoneMessage(Message *message, local_id id, balance_t balance);

#endif //LAB1_MESSAGE_H
