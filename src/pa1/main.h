#ifndef LAB1_MAIN_H
#define LAB1_MAIN_H

#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#include "pa1_starter_code/pa1.h"
#include "pa1_starter_code/ipc.h"
#include "pa1_starter_code/common.h"

#define DESC_COUNT 2
#define READ_DESC 0
#define WRITE_DESC 1
#define MAX_PROC 11

typedef struct {
    int pipes[MAX_PROC][MAX_PROC][DESC_COUNT];
} ProcessPipes;

typedef struct {
    ProcessPipes pipesData;
    int procCount;
    local_id localId;
} MetaData;

#endif //LAB1_MAIN_H
