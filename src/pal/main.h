#ifndef LAB1_MAIN_H
#define LAB1_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#include "pa1_starter_code/pa1.h"
#include "pa1_starter_code/ipc.h"
#include "pa1_starter_code/common.h"

static const char *const log_open_pipe_descr_r =
        "pipe descriptor number %d has open for reading in process %1d (pid %5d, parent %5d)\n";
static const char *const log_open_pipe_descr_w =
        "pipe descriptor number %d has open for writing in process %1d (pid %5d, parent %5d)\n";
static const char *const log_close_pipe_descr =
        "pipe descriptor number %d has close in process %1d (pid %5d, parent %5d)\n";




#endif //LAB1_MAIN_H
