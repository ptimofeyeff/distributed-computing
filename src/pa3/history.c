#include "history.h"

void buildBalanceState(BalanceState *balanceState, balance_t balance, balance_t pending) {
    balanceState->s_balance = balance;
    balanceState->s_balance_pending_in = pending;
    balanceState->s_time = get_lamport_time();
}


void commitBalanceState(BalanceState *balanceState, BalanceHistory *balanceHistory, timestamp_t lastCommitTime, int savePending) {
    // write history
    for (int i = lastCommitTime; i < get_lamport_time(); ++i) {
        balanceHistory->s_history[i].s_time = i;
        balanceHistory->s_history[i].s_balance = balanceHistory->s_history[lastCommitTime].s_balance;
        balanceHistory->s_history[i].s_balance_pending_in = balanceHistory->s_history[lastCommitTime].s_balance_pending_in;
    }
    // write now
    balanceHistory->s_history[get_lamport_time()].s_time = get_lamport_time();
    balanceHistory->s_history[get_lamport_time()].s_balance = balanceState->s_balance;
    if (savePending) {
        balanceHistory->s_history[get_lamport_time()].s_balance_pending_in = balanceState->s_balance_pending_in;
    } else {
        balanceHistory->s_history[get_lamport_time()].s_balance_pending_in = balanceHistory->s_history[lastCommitTime].s_balance_pending_in;
    }
}
