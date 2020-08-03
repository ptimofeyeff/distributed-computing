#ifndef LAB1_LOGS_H
#define LAB1_LOGS_H

#include "pa1_starter_code/ipc.h"
#include "stdio.h"
#include <unistd.h>
#include "pa1_starter_code/pa1.h"

FILE *eventsLogs;

static const char *const log_open_pipe_descr =
        "pipe descriptor number %d has open for %s from %d to %d\n";
static const char *const log_close_pipe_descr =
        "pipe descriptor number %d has close in process %d\n";

void logStarted(local_id, char * q);

void logDone(local_id, char *);

void logReceiveStart(local_id, char *);

void logReceiveDone(local_id, char *);

void logOpenDescriptor(int, char *, int, int);

void logCloseDescriptor(int, local_id);

void printMessage(Message *message, local_id id);



#endif //LAB1_LOGS_H
