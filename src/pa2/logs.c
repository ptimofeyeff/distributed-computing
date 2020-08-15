#include "logs.h"
#include "descriptors.h"


void logStarted(local_id id, char *startedMessage, timestamp_t timestamp, balance_t balance ) {
    sprintf(startedMessage, log_started_fmt, timestamp, id, getpid(), getppid(), balance);
    fprintf(eventsLogs, "%s", startedMessage);
    fflush(eventsLogs);
    printf("%s", startedMessage);
    fflush(stdout);
}

void logDone(local_id id, char *doneMessage, timestamp_t timestamp, balance_t balance) {
    sprintf(doneMessage, log_done_fmt, timestamp, id, balance);
    fprintf(eventsLogs, "%s", doneMessage);
    fflush(eventsLogs);
    printf("%s", doneMessage);
    fflush(stdout);
}

void logReceiveStart(local_id id, char *receiveStartMsg, timestamp_t timestamp) {
    sprintf(receiveStartMsg, log_received_all_started_fmt, timestamp, id);
    fprintf(eventsLogs, "%s", receiveStartMsg);
    fflush(eventsLogs);
    printf("%s", receiveStartMsg);
    fflush(stdout);
}

void logReceiveDone(local_id id, char *receiveDoneMsg, timestamp_t timestamp) {
    sprintf(receiveDoneMsg, log_received_all_done_fmt, id, timestamp);
    fprintf(eventsLogs, "%s", receiveDoneMsg);
    fflush(eventsLogs);
    printf("%s", receiveDoneMsg);
    fflush(stdout);
}

void logOpenDescriptor(int fd, char *type, int from, int to) {
    fprintf(pipesLogs, log_open_pipe_descr, fd, type, from, to);
    fflush(pipesLogs);
    printf(log_open_pipe_descr, fd, type, from, to);
    fflush(stdout);
}

void logCloseDescriptor(int fd, local_id id) {
    fprintf(pipesLogs, log_close_pipe_descr, fd, id);
    fflush(pipesLogs);
    printf(log_close_pipe_descr, fd, id);
    fflush(stdout);
}

void logTransferIn(timestamp_t timestamp, local_id src, balance_t amount, local_id dst) {
    fprintf(eventsLogs, log_transfer_in_fmt, timestamp, dst, amount, src);
    fflush(eventsLogs);
    printf(log_transfer_in_fmt, timestamp, src, amount, dst);
    fflush(stdout);
}

void logTransferOut(timestamp_t timestamp, local_id src, balance_t amount, local_id dst) {
    fprintf(eventsLogs, log_transfer_out_fmt, timestamp, src, amount, dst);
    fflush(eventsLogs);
    printf(log_transfer_out_fmt, timestamp, src, amount, dst);
    fflush(stdout);
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
    fflush(stdout);
}
