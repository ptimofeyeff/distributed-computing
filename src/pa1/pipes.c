#include "pipes.h"

void initPipes(MetaData *metaData) {
    pipesLogs = fopen(pipes_log, "w");
    for (int i = 0; i < metaData->pipesData.procCount; ++i) {
        for (int j = 0; j < metaData->pipesData.procCount; ++j) {
            if (i != j) {
                pipe(metaData->pipesData.pipes[i][j]);
                fprintf(pipesLogs, log_open_pipe_descr_r, metaData->pipesData.pipes[i][j][READ_DESC], *metaData->localId, getpid(), getppid());
                fprintf(pipesLogs, log_open_pipe_descr_w, metaData->pipesData.pipes[i][j][WRITE_DESC], *metaData->localId, getpid(), getppid());
                fflush(pipesLogs);
                printf(log_open_pipe_descr_r, metaData->pipesData.pipes[i][j][READ_DESC], *metaData->localId, getpid(), getppid());
                printf(log_open_pipe_descr_w, metaData->pipesData.pipes[i][j][WRITE_DESC], *metaData->localId, getpid(), getppid());
            }
        }

    }
}

void finalizePipes (MetaData *metaData) {
    for (int i = 0; i < metaData->pipesData.procCount; ++i) {
        for (int j = 0; j < metaData->pipesData.procCount; ++j) {
            if (i != j) {
                close(metaData->pipesData.pipes[i][j][READ_DESC]);
                close(metaData->pipesData.pipes[i][j][WRITE_DESC]);
                fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][j][READ_DESC], *metaData->localId, getpid(), getppid());
                fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][j][WRITE_DESC], *metaData->localId, getpid(),getppid());
                printf(log_close_pipe_descr, metaData->pipesData.pipes[i][j][READ_DESC], *metaData->localId, getpid(), getppid());
                printf(log_close_pipe_descr, metaData->pipesData.pipes[i][j][WRITE_DESC], *metaData->localId, getpid(), getppid());
            }
        }
    }
    fclose(pipesLogs);
}
