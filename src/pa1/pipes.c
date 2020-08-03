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
                printf(log_open_pipe_descr_w, processPipes->pipes[i][j][WRITE_DESC],i, j, PARENT_ID);
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
                fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC]);
                fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC]);
                printf(log_close_pipe_descr, processPipes->pipes[i][j][READ_DESC]);
                printf(log_close_pipe_descr, processPipes->pipes[i][j][WRITE_DESC]);
            }
        }
    }
}


void initParentPipes(ProcessPipes *pipesData, int procCount) {
    for (int i = 1; i < procCount; ++i) {
        pipe(pipesData->pipes[i][PARENT_ID]);
        close(pipesData->pipes[i][PARENT_ID][WRITE_DESC]);
        fprintf(pipesLogs, log_open_pipe_descr_r, pipesData->pipes[i][0][READ_DESC], i, PARENT_ID, PARENT_ID);
        fflush(pipesLogs);
        printf(log_open_pipe_descr_r, pipesData->pipes[i][0][READ_DESC], i, PARENT_ID, PARENT_ID);
        fflush(stdout);
    }
}

void closeParentPipes(ProcessPipes *processPipes, int procCount) {
    for (int i = 1; i < procCount; ++i) {
        close(processPipes->pipes[i][PARENT_ID][READ_DESC]);
        fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][PARENT_ID][READ_DESC]);
        fflush(pipesLogs);
        printf(log_close_pipe_descr, processPipes->pipes[i][0][READ_DESC]);
    }
}

void initChildPipes(ProcessPipes *processPipes, int procCount, local_id id) {
    for (int i = 0; i < procCount; ++i) {
        if (i != id) {
            pipe(processPipes->pipes[id][i]);
            fprintf(pipesLogs, log_open_pipe_descr_r, processPipes->pipes[i][id][READ_DESC], i, id, id);
            fprintf(pipesLogs, log_open_pipe_descr_w, processPipes->pipes[id][i][WRITE_DESC], id, i, id);
            fflush(pipesLogs);
            printf(log_open_pipe_descr_r, processPipes->pipes[i][id][READ_DESC], i, id, id);
            printf(log_open_pipe_descr_w, processPipes->pipes[id][i][WRITE_DESC], id, i, id);
            fflush(stdout);
        }
    }
}

void closeChildPipes(ProcessPipes *processPipes, int procCount, local_id id) {
    for (int i = 0; i < procCount; ++i) {
        if (i != id) {
            close(processPipes->pipes[id][i][READ_DESC]);
            close(processPipes->pipes[id][i][WRITE_DESC]);
            fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][id][READ_DESC]);
            fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[id][i][WRITE_DESC]);
            fflush(pipesLogs);
            printf(log_close_pipe_descr, processPipes->pipes[i][id][READ_DESC]);
            printf(log_close_pipe_descr, processPipes->pipes[id][i][WRITE_DESC]);
        }
    }
}
