#ifndef LAB1_LOGS_H
#define LAB1_LOGS_H

#include "pa1_starter_code/ipc.h"
#include "stdio.h"
#include <unistd.h>
#include "pa1_starter_code/pa1.h"

FILE *eventsLogs;

void logStarted(local_id, char * q);

void logDone(local_id, char *);

void logReceiveStart(local_id, char *);

void logReceiveDone(local_id, char *);

void printMessage(Message *message, local_id id);



#endif //LAB1_LOGS_H
