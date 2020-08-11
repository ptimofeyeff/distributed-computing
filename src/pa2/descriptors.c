#include "descriptors.h"

void openPipes(BranchDescriptors *branchDescriptors, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                pipe(branchDescriptors->descriptors[i][j]);
                //logOpenDescriptor(branchDescriptors->descriptors[i][j][READ_DESC], "reading", i, j);
                //logOpenDescriptor(branchDescriptors->descriptors[i][j][WRITE_DESC], "writing", i, j);
            }
        }
    }
}

void closePipes(BranchDescriptors *branchDescriptors, int procCount, local_id id) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                close(branchDescriptors->descriptors[i][j][READ_DESC]);
                close(branchDescriptors->descriptors[i][j][WRITE_DESC]);
                //logCloseDescriptor(branchDescriptors->descriptors[i][j][READ_DESC], id);
                //logCloseDescriptor(branchDescriptors->descriptors[i][j][WRITE_DESC], id);
            }
        }
    }
}


void closeOtherParentDescriptors(BranchDescriptors *branchDescriptors, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j){
                if (j != PARENT_ID && i != PARENT_ID) {
                    close(branchDescriptors->descriptors[i][j][READ_DESC]);
                    close(branchDescriptors->descriptors[i][j][WRITE_DESC]);
                    //logCloseDescriptor(branchDescriptors->descriptors[i][j][READ_DESC], PARENT_ID);
                    //logCloseDescriptor(branchDescriptors->descriptors[i][j][WRITE_DESC], PARENT_ID);
                } else {
                    if (j == PARENT_ID) {
                        close(branchDescriptors->descriptors[i][j][WRITE_DESC]);
                        //logCloseDescriptor(branchDescriptors->descriptors[i][j][WRITE_DESC], PARENT_ID);
                    } else {
                        close(branchDescriptors->descriptors[i][j][READ_DESC]);
                        //logCloseDescriptor(branchDescriptors->descriptors[i][j][READ_DESC], PARENT_ID);
                    }
                }
            }
        }
    }
}

void closeOtherChildDescriptors(BranchDescriptors *branchDescriptors, local_id id, int procCount) {
    for (int i = 0; i < procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            if (i != j) {
                if (i == id && i > 0) {
                    close(branchDescriptors->descriptors[i][j][READ_DESC]);
                    //logCloseDescriptor(branchDescriptors->descriptors[i][j][READ_DESC], id);
                } else {
                    if (j == id) {
                        close(branchDescriptors->descriptors[i][j][WRITE_DESC]);
                        //logCloseDescriptor(branchDescriptors->descriptors[i][j][WRITE_DESC], id);
                    } else {
                        close(branchDescriptors->descriptors[i][j][READ_DESC]);
                        close(branchDescriptors->descriptors[i][j][WRITE_DESC]);
                        //logCloseDescriptor(branchDescriptors->descriptors[i][j][READ_DESC], id);
                        //logCloseDescriptor(branchDescriptors->descriptors[i][j][WRITE_DESC], id);
                    }
                }
            }
        }
    }
}

void setNonBlocking(BranchDescriptors *branchDescriptors, int procCount) {
    for (int i = 0; i <procCount; ++i) {
        for (int j = 0; j < procCount; ++j) {
            fcntl(branchDescriptors->descriptors[i][j][READ_DESC], F_SETFL, O_NONBLOCK);
            fcntl(branchDescriptors->descriptors[i][j][WRITE_DESC], F_SETFL, O_NONBLOCK);
        }
    }
}
