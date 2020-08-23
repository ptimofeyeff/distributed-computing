#ifndef LAB1_HISTORY_H
#define LAB1_HISTORY_H

#include "banking.h"

void buildBalanceState(BalanceState *, balance_t, balance_t);

void commitBalanceState(BalanceState *, BalanceHistory *, timestamp_t, int);

#endif //LAB1_HISTORY_H
