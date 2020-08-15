#include "history.h"
#include "stdio.h"

void buildBalanceState(BalanceState *balanceState, balance_t balance) {
    balanceState->s_balance = balance;
    balanceState->s_balance_pending_in = 0;
    balanceState->s_time = get_physical_time();
}


void commitBalanceState(BalanceState *balanceState, BalanceHistory *balanceHistory, timestamp_t lastCommitTime, timestamp_t currentTimeStamp) {
    for (int i = lastCommitTime + 1; i <= currentTimeStamp; ++i) {
        balanceHistory->s_history[i] = *balanceState;
    }
}
