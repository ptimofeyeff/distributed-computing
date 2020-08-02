#ifndef LAB1_PIPES_H
#define LAB1_PIPES_H

#include "main.h"
#include "stdio.h"
#include "unistd.h"

FILE *pipesLogs;

void initPipes(MetaData *);

void finalizePipes(MetaData *);


#endif //LAB1_PIPES_H
