#include "pipes.h"
#include "logs.h"

void initParentPipes(ProcessPipes *pipesData, int procCount) {
    for (int i = 1; i < procCount; ++i) {
        pipe(pipesData->pipes[i][PARENT_ID]);
        close(pipesData->pipes[i][PARENT_ID][WRITE_DESC]);
        fprintf(pipesLogs, log_open_pipe_descr_r, pipesData->pipes[i][0][READ_DESC], i, PARENT_ID);
        fflush(pipesLogs);
        printf(log_open_pipe_descr_r, pipesData->pipes[i][0][READ_DESC], i, PARENT_ID);
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
    // init child write_desc
    for (int i = 0; i < procCount; ++i) {
        if (i != id) {
            pipe(processPipes->pipes[id][i]);
            close(processPipes->pipes[id][i][READ_DESC]);
            fprintf(pipesLogs, log_open_pipe_descr_w, processPipes->pipes[id][i][WRITE_DESC], id, i);
            fflush(pipesLogs);
            printf(log_open_pipe_descr_w, processPipes->pipes[id][i][WRITE_DESC], id, i);
        }
    }

    // init child read_desc
    for (int i = 1; i < procCount; ++i) {
        if (i != id) {
            pipe(processPipes->pipes[id][i]);
            close(processPipes->pipes[i][id][WRITE_DESC]);
            fprintf(pipesLogs, log_open_pipe_descr_r, processPipes->pipes[i][id][READ_DESC], i, id);
            fflush(pipesLogs);
            printf(log_open_pipe_descr_r, processPipes->pipes[i][id][READ_DESC], i, id);
        }
    }

}

void closeChildPipes(ProcessPipes *processPipes, int procCount, local_id id) {
    // close child write_desc
    for (int i = 0; i < procCount; ++i) {
        if (i != id) {
            close(processPipes->pipes[id][i][WRITE_DESC]);
            fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[id][i][WRITE_DESC]);
            fflush(pipesLogs);
            printf(log_close_pipe_descr, processPipes->pipes[id][i][WRITE_DESC]);
        }
    }
    // close child read_desc
    for (int i = 1; i < procCount; ++i) {
        if (i != id) {
            close(processPipes->pipes[id][i][READ_DESC]);
            fprintf(pipesLogs, log_close_pipe_descr, processPipes->pipes[i][id][READ_DESC]);
            fflush(pipesLogs);
            printf(log_close_pipe_descr, processPipes->pipes[i][id][READ_DESC]);
        }
    }
}
