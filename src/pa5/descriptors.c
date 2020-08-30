#include "descriptors.h"
#include <fcntl.h>
#include <unistd.h>
#include "logs.h"

void openPipes(TopologyDescriptors *descriptors, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                pipe(descriptors->descriptors[i][j]);
                logOpenDescriptor(descriptors->descriptors[i][j][READ_DESC], "reading", i, j);
                logOpenDescriptor(descriptors->descriptors[i][j][WRITE_DESC], "writing", i, j);
                fcntl(descriptors->descriptors[i][j][READ_DESC], F_SETFL, O_NONBLOCK);
                fcntl(descriptors->descriptors[i][j][WRITE_DESC], F_SETFL, O_NONBLOCK);
            }
        }
    }
}

void closePipes(TopologyDescriptors *descriptors, int procCount, local_id id) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                close(descriptors->descriptors[i][j][READ_DESC]);
                close(descriptors->descriptors[i][j][WRITE_DESC]);
                logCloseDescriptor(descriptors->descriptors[i][j][READ_DESC], id);
                logCloseDescriptor(descriptors->descriptors[i][j][WRITE_DESC], id);
            }
        }
    }
}


void closeOtherParentDescriptors(TopologyDescriptors *descriptors, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j){
                if (j != PARENT_ID && i != PARENT_ID) {
                    close(descriptors->descriptors[i][j][READ_DESC]);
                    close(descriptors->descriptors[i][j][WRITE_DESC]);
                    logCloseDescriptor(descriptors->descriptors[i][j][READ_DESC], PARENT_ID);
                    logCloseDescriptor(descriptors->descriptors[i][j][WRITE_DESC], PARENT_ID);
                } else {
                    if (j == PARENT_ID) {
                        close(descriptors->descriptors[i][j][WRITE_DESC]);
                        logCloseDescriptor(descriptors->descriptors[i][j][WRITE_DESC], PARENT_ID);
                    } else {
                        close(descriptors->descriptors[i][j][READ_DESC]);
                        logCloseDescriptor(descriptors->descriptors[i][j][READ_DESC], PARENT_ID);
                    }
                }
            }
        }
    }
}

void closeOtherChildDescriptors(TopologyDescriptors *descriptors, local_id id, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                if (i == id && i > 0) {
                    close(descriptors->descriptors[i][j][READ_DESC]);
                    logCloseDescriptor(descriptors->descriptors[i][j][READ_DESC], id);
                } else {
                    if (j == id) {
                        close(descriptors->descriptors[i][j][WRITE_DESC]);
                        logCloseDescriptor(descriptors->descriptors[i][j][WRITE_DESC], id);
                    } else {
                        close(descriptors->descriptors[i][j][READ_DESC]);
                        close(descriptors->descriptors[i][j][WRITE_DESC]);
                        logCloseDescriptor(descriptors->descriptors[i][j][READ_DESC], id);
                        logCloseDescriptor(descriptors->descriptors[i][j][WRITE_DESC], id);
                    }
                }
            }
        }
    }
}
