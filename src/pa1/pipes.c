#include "pipes.h"
#include "logs.h"

void openPipes(ProcessPipes *processPipes, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                pipe(processPipes->pipes[i][j]);
                logOpenDescriptor(processPipes->pipes[i][j][READ_DESC], "reading", i, j);
                logOpenDescriptor(processPipes->pipes[i][j][WRITE_DESC], "writing", i, j);
            }
        }
    }
}

void closePipes(ProcessPipes *processPipes, int procCount, local_id id) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                close(processPipes->pipes[i][j][READ_DESC]);
                close(processPipes->pipes[i][j][WRITE_DESC]);
                logCloseDescriptor(processPipes->pipes[i][j][READ_DESC], id);
                logCloseDescriptor(processPipes->pipes[i][j][WRITE_DESC], id);
            }
        }
    }
}


void closeOtherParentDescriptors(ProcessPipes *processPipes, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j && j != PARENT_ID) {
                close(processPipes->pipes[i][j][READ_DESC]);
                close(processPipes->pipes[i][j][WRITE_DESC]);
                logCloseDescriptor(processPipes->pipes[i][j][READ_DESC], PARENT_ID);
                logCloseDescriptor(processPipes->pipes[i][j][WRITE_DESC], PARENT_ID);
            }
        }
    }
}

void closeOtherChildDescriptors(ProcessPipes *processPipes, local_id id, int procCount) {

    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                if (i == id) {
                    close(processPipes->pipes[i][j][READ_DESC]);
                    fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], PARENT_ID);
                    printf(log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], PARENT_ID);
                } else {
                    if (j == id && i > 0) {
                        close(processPipes->pipes[i][j][WRITE_DESC]);
                        fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], PARENT_ID);
                        printf(log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], PARENT_ID);
                    } else {
                        close(processPipes->pipes[i][j][READ_DESC]);
                        close(processPipes->pipes[i][j][WRITE_DESC]);
                        fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], PARENT_ID);
                        fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], PARENT_ID);
                        printf(log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], PARENT_ID);
                        printf(log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], PARENT_ID);
                    }
                }
            }
        }
    }
}
