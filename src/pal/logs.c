#include "logs.h"


void logStarted(local_id id, char *startedMessage) {
    sprintf(startedMessage, log_started_fmt, id, getpid(), getppid());
    fprintf(eventsLogs, "%s", startedMessage);
    fflush(eventsLogs);
    printf("%s", startedMessage);
}

void logDone(local_id id, char *doneMessage) {
    sprintf(doneMessage, log_done_fmt, id);
    fprintf(eventsLogs, "%s", doneMessage);
    fflush(eventsLogs);
    printf("%s", doneMessage);
}

void logReceiveStart(local_id id, char *receiveStartMsg) {
    sprintf(receiveStartMsg, log_received_all_started_fmt, id);
    fprintf(eventsLogs, "%s", receiveStartMsg);
    fflush(eventsLogs);
    printf("%s", receiveStartMsg);
}

void logReceiveDone(local_id id, char *receiveDoneMsg) {
    sprintf(receiveDoneMsg, log_received_all_done_fmt, id);
    fprintf(eventsLogs, "%s", receiveDoneMsg);
    fflush(eventsLogs);
    printf("%s", receiveDoneMsg);
}

void printMessage(Message *message, local_id id) {
    printf("\n\nprocess with id %d receive:\n"
           "message magic = %d\n"
           "message local time = %d\n"
           "message payload length = %d\n"
           "message type = %d\n"
           "message payload = %s\n\n",
           id, message->s_header.s_magic, message->s_header.s_local_time,  message->s_header.s_payload_len,
           message->s_header.s_type, message->s_payload);
}
