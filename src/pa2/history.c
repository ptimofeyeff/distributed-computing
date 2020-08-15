#include "history.h"

void buildBalanceState(BalanceState *balanceState, local_id id, balance_t balance) {
    balanceState->s_balance = balance;
    balanceState->s_balance_pending_in = 0;
    balanceState->s_time = get_physical_time();
}
