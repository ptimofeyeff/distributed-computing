#include "pipes.h"
#include "logs.h"

void openPipes(ProcessPipes *processPipes, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                pipe(processPipes->pipes[i][j]);
                fprintf(pipesLogs, log_open_pipe_descr_r, processPipes->pipes[i][j][READ_DESC], i, j, PARENT_ID);
                fprintf(pipesLogs, log_open_pipe_descr_w, processPipes->pipes[i][j][WRITE_DESC], i, j, PARENT_ID);
                fflush(pipesLogs);
                printf(log_open_pipe_descr_r, processPipes->pipes[i][j][READ_DESC], i, j, PARENT_ID);
                printf(log_open_pipe_descr_w, processPipes->pipes[i][j][WRITE_DESC], i, j, PARENT_ID);
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
                fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], id);
                fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], id);
                printf(log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], id);
                printf(log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], id);
            }
        }
    }
}


void closeOtherParentDescriptors(ProcessPipes *processPipes, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                if (j == PARENT_ID) {
                    close(processPipes->pipes[i][j][WRITE_DESC]);
                    fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], PARENT_ID);
                    printf(log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], PARENT_ID);
                } else {
                    close(processPipes->pipes[i][j][READ_DESC]);
                    fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], PARENT_ID);
                    printf(log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], PARENT_ID);
                    close(processPipes->pipes[i][j][WRITE_DESC]);
                    fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], PARENT_ID);
                    printf(log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], PARENT_ID);
                }
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
