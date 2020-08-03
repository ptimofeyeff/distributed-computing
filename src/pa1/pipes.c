#include "pipes.h"
#include "logs.h"

void openPipes(ProcessPipes *processPipes, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j <procCount; ++j) {
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

void closePipes(ProcessPipes *processPipes, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                close(processPipes->pipes[i][j][READ_DESC]);
                close(processPipes->pipes[i][j][WRITE_DESC]);
                fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], getpid());
                fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], getpid());
                printf(log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC], getpid());
                printf(log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC], getpid());
            }
        }
    }
}


void initParentPipes(ProcessPipes *processesPipes, ProcessPipes *parentPipes, int procCount) {
    for (int i = 1; i <procCount; ++i) {
        parentPipes->pipes[i][PARENT_ID][READ_DESC] = processesPipes->pipes[i][PARENT_ID][READ_DESC];
    }
}

void initChildPipes(ProcessPipes *processesPipes, ProcessPipes *childPipes, local_id childId, int procCount) {
    for (int i = 0; i <procCount ; ++i) {
        for (int j = 0; j <procCount; ++j) {
            if (i != j) {
                if (i != 0) {
                    childPipes->pipes[i][childId][READ_DESC] = processesPipes->pipes[i][childId][READ_DESC];
                }
                childPipes->pipes[childId][i][WRITE_DESC] = processesPipes->pipes[childId][i][WRITE_DESC];
            }
        }
    }
}
