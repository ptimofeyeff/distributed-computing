#ifndef LAB1_PIPES_H
#define LAB1_PIPES_H

#include "main.h"
#include "stdio.h"
#include "unistd.h"

FILE *pipesLogs;

void openPipes(MetaData *);

void initParentPipes(MetaData *);

void initChildPipes(MetaData *);

void closeParentPipes(MetaData *);

void closeChildPipes(MetaData *);

void closePipes(MetaData *);

#endif //LAB1_PIPES_H
