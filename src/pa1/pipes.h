#ifndef LAB1_PIPES_H
#define LAB1_PIPES_H

#include "main.h"
#include "stdio.h"
#include "unistd.h"

FILE *pipesLogs;

void openPipes(ProcessPipes *, int);

void closePipes(ProcessPipes *, int, local_id);

void closeOtherChildDescriptors(ProcessPipes *, local_id id, int);

void closeOtherParentDescriptors(ProcessPipes *, int);

#endif //LAB1_PIPES_H
