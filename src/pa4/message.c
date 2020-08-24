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
