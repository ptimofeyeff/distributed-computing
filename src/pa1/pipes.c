#include "pipes.h"

void openPipes(MetaData *metaData) {
    for (int i = 0; i < metaData->pipesData.procCount; ++i) {
        for (int j = 0; j < metaData->pipesData.procCount; ++j) {
            if (i != j) {
                pipe(metaData->pipesData.pipes[i][j]);
                fprintf(pipesLogs, log_open_pipe_descr_r, metaData->pipesData.pipes[i][j][READ_DESC], metaData->localId, getpid(), getppid());
                fprintf(pipesLogs, log_open_pipe_descr_w, metaData->pipesData.pipes[i][j][WRITE_DESC], metaData->localId, getpid(), getppid());
                fflush(pipesLogs);
                printf(log_open_pipe_descr_r, metaData->pipesData.pipes[i][j][READ_DESC], metaData->localId, getpid(), getppid());
                printf(log_open_pipe_descr_w, metaData->pipesData.pipes[i][j][WRITE_DESC], metaData->localId, getpid(), getppid());
            }
        }
    }
}

void closePipes (MetaData *metaData) {
    for (int i = 0; i < metaData->pipesData.procCount; ++i) {
        for (int j = 0; j < metaData->pipesData.procCount; ++j) {
            if (i != j) {
                close(metaData->pipesData.pipes[i][j][READ_DESC]);
                close(metaData->pipesData.pipes[i][j][WRITE_DESC]);
                fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][j][READ_DESC], metaData->localId, getpid(), getppid());
                fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][j][WRITE_DESC], metaData->localId, getpid(),getppid());
                printf(log_close_pipe_descr, metaData->pipesData.pipes[i][j][READ_DESC], metaData->localId, getpid(), getppid());
                printf(log_close_pipe_descr, metaData->pipesData.pipes[i][j][WRITE_DESC], metaData->localId, getpid(), getppid());
            }
        }
    }
}


void initParentPipes(MetaData *metaData) {
    for (int i = 1; i <metaData->pipesData.procCount; ++i) {
        pipe(metaData->pipesData.pipes[i][0]);
        close(metaData->pipesData.pipes[i][0][WRITE_DESC]);
        fprintf(pipesLogs, log_open_pipe_descr_r, metaData->pipesData.pipes[i][0][READ_DESC], metaData->localId, getpid(), getppid());
        fflush(pipesLogs);
        printf(log_open_pipe_descr_r, metaData->pipesData.pipes[i][0][READ_DESC], metaData->localId, getpid(), getppid());
    }
}

void closeParentPipes(MetaData *metaData) {
    for (int i = 1; i <metaData->pipesData.procCount; ++i) {
        close(metaData->pipesData.pipes[i][0][READ_DESC]);
        fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][0][READ_DESC], metaData->localId, getpid(), getppid());
        fflush(pipesLogs);
        printf(log_close_pipe_descr, metaData->pipesData.pipes[i][0][READ_DESC], metaData->localId, getpid(), getppid());
    }
}

void initChildPipes(MetaData *metaData) {
    // init child write_desc
    for (int i = 0; i <metaData->pipesData.procCount; ++i) {
        if (i != metaData->localId) {
            pipe(metaData->pipesData.pipes[metaData->localId][i]);
            close(metaData->pipesData.pipes[metaData->localId][i][READ_DESC]);
            fprintf(pipesLogs, log_open_pipe_descr_w, metaData->pipesData.pipes[metaData->localId][i][WRITE_DESC],
                    metaData->localId, getpid(), getppid());
            fflush(pipesLogs);
            printf(log_open_pipe_descr_w, metaData->pipesData.pipes[metaData->localId][i][WRITE_DESC], metaData->localId, getpid(), getppid());
        }
    }

    // init child read_desc
    for (int i = 1; i <metaData->pipesData.procCount; ++i) {
        if (i != metaData->localId) {
            pipe(metaData->pipesData.pipes[metaData->localId][i]);
            close(metaData->pipesData.pipes[i][metaData->localId][WRITE_DESC]);
            fprintf(pipesLogs, log_open_pipe_descr_r, metaData->pipesData.pipes[i][metaData->localId][READ_DESC],
                    metaData->localId, getpid(), getppid());
            fflush(pipesLogs);
            printf(log_open_pipe_descr_r, metaData->pipesData.pipes[i][metaData->localId][READ_DESC], metaData->localId, getpid(), getppid());
        }
    }

}

void closeChildPipes(MetaData *metaData) {
    // close child write_desc
    for (int i = 0; i <metaData->pipesData.procCount; ++i) {
        if (i != metaData->localId) {
            close(metaData->pipesData.pipes[metaData->localId][i][WRITE_DESC]);
            fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[metaData->localId][i][WRITE_DESC],
                    metaData->localId, getpid(), getppid());
            fflush(pipesLogs);
            printf(log_close_pipe_descr, metaData->pipesData.pipes[metaData->localId][i][WRITE_DESC], metaData->localId, getpid(), getppid());
        }
    }

    // close child read_desc
    for (int i = 1; i <metaData->pipesData.procCount; ++i) {
        if (i != metaData->localId) {
            close(metaData->pipesData.pipes[metaData->localId][i][READ_DESC]);
            fprintf(pipesLogs, log_close_pipe_descr, metaData->pipesData.pipes[i][metaData->localId][READ_DESC],
                    metaData->localId, getpid(), getppid());
            fflush(pipesLogs);
            printf(log_close_pipe_descr, metaData->pipesData.pipes[i][metaData->localId][READ_DESC], metaData->localId, getpid(), getppid());
        }
    }
}
