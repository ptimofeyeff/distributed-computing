#ifndef LAB1_PIPES_H
#define LAB1_PIPES_H

#include "main.h"
#include "stdio.h"
#include "unistd.h"

FILE *pipesLogs;

void openPipes(ProcessPipes *, int);

void closePipes(ProcessPipes *, int);

void initParentPipes(ProcessPipes *, int);

void initChildPipes(ProcessPipes *, int, local_id);

void closeParentPipes(ProcessPipes *, int);

void closeChildPipes(ProcessPipes *, int, local_id);

#endif //LAB1_PIPES_H
