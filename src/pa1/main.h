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

static const char *const log_open_pipe_descr_r =
        "pipe descriptor number %d has open for reading in process %1d (pid %5d, parent %5d)\n";
static const char *const log_open_pipe_descr_w =
        "pipe descriptor number %d has open for writing in process %1d (pid %5d, parent %5d)\n";
static const char *const log_close_pipe_descr =
        "pipe descriptor number %d has close in process %1d (pid %5d, parent %5d)\n";

typedef struct {
    int procCount;
    int pipes[MAX_PROC][MAX_PROC][DESC_COUNT];
} PipesData;

typedef struct {
    PipesData pipesData;
    local_id localId;
} __attribute__((packed)) MetaData;

#endif //LAB1_MAIN_H
