#ifndef LAB1_LOGS_H
#define LAB1_LOGS_H

#include <stdio.h>
#include "ipc.h"
#include "banking.h"

FILE *eventsLogs;
FILE *pipesLogs;

static const char *const log_open_pipe_descr =
        "pipe descriptor number %d has open for %s from %d to %d\n";
static const char *const log_close_pipe_descr =
        "pipe descriptor number %d has close in process %d\n";

void logStarted(local_id, balance_t);

void logDone(local_id, balance_t);

void logReceiveStart(local_id);

void logReceiveDone(local_id);

void logOpenDescriptor(int, char *, int, int);

void logCloseDescriptor(int, local_id);

void logTransferIn(local_id src, balance_t, local_id dst);

void logTransferOut(local_id src, balance_t, local_id dst);

void printMessage(Message *message, local_id id);



#endif //LAB1_LOGS_H
