#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "message.h"
#include "pa2345.h"


void buildStartMessage(Message *message, local_id id, balance_t balance) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = STARTED;
    message->s_header.s_local_time = get_lamport_time();
    sprintf(message->s_payload, log_started_fmt, message->s_header.s_local_time, id, getpid(), getpid(), balance);
    message->s_header.s_payload_len = strlen(message->s_payload);
}

void buildDoneMessage(Message *message, local_id id, balance_t balance) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = DONE;
    message->s_header.s_local_time = get_lamport_time();
    sprintf(message->s_payload, log_done_fmt, message->s_header.s_local_time, id, balance);
    message->s_header.s_payload_len = strlen(message->s_payload);
}

void buildTransferMessage(Message *message, TransferOrder *transferOrder) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = TRANSFER;
    message->s_header.s_local_time = get_lamport_time();
    message->s_header.s_payload_len = sizeof *transferOrder;
    memcpy(message->s_payload, transferOrder, sizeof *transferOrder);
}

void buildAckMessage(Message *message) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_payload_len = 0;
    message->s_header.s_type = ACK;
    message->s_header.s_local_time = get_lamport_time();
}

void buildStopMessage(Message *message) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_payload_len = 0;
    message->s_header.s_type = STOP;
    message->s_header.s_local_time = get_lamport_time();
}

void buildHistoryMessage(Message *message, BalanceHistory *balanceHistory) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = BALANCE_HISTORY;
    message->s_header.s_payload_len = sizeof *balanceHistory;
    message->s_header.s_local_time = get_lamport_time();
    memcpy(message->s_payload, balanceHistory, sizeof *balanceHistory);
}
