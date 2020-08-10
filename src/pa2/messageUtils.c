#include "messageUtils.h"
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

void receiveFromAll(BranchData *branchData, Message *message) {
    for (int i = 1; i < branchData->branchCount; ++i) {
        if (i != branchData->id) {
            receive(branchData, i, message);
            //printMessage(message, metaData->localId);
        }
    }
}
