#include "message.h"
#include <string.h>

void buildEmptyMessage(Message *message, char *payload, MessageType type) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = type;
    message->s_header.s_local_time = get_physical_time();
    message->s_header.s_payload_len = strlen(payload);
    strcpy(message->s_payload, payload);
}

void buildTransferMessage(Message *message, TransferOrder *transferOrder) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = TRANSFER;
    message->s_header.s_local_time = get_physical_time();
    message->s_header.s_payload_len = sizeof *transferOrder;
    memcpy(message->s_payload, transferOrder, sizeof *transferOrder);
}

void buildAckMessage(Message *message) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_payload_len = 0;
    message->s_header.s_type = ACK;
    message->s_header.s_local_time = get_physical_time();
}

void buildStopMessage(Message *message) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_payload_len = 0;
    message->s_header.s_type = STOP;
    message->s_header.s_local_time = get_physical_time();
}

void buildHistoryMessage(Message *message, BalanceHistory *balanceHistory) {
    message->s_header.s_magic = MESSAGE_MAGIC;
    message->s_header.s_type = BALANCE_HISTORY;
    message->s_header.s_payload_len = sizeof *balanceHistory;
    message->s_header.s_local_time = get_physical_time();
    memcpy(message->s_payload, balanceHistory, sizeof *balanceHistory);
}

// "синхронная" функция получения сообщений от всех процессов
void receiveFromAll(BranchData *branchData, Message *message) {
    for (int i = 1; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            while (1) {
                if (receive(branchData, i, message) == 0) {
                    break;
                }
                //sleep(1);
            }
        }
    }
}
