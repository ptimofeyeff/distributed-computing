#ifndef LAB1_DESCRIPTORS_H
#define LAB1_DESCRIPTORS_H

#include "stdio.h"
#include "unistd.h"
#include <fcntl.h>
#include "branch.h"

FILE *pipesLogs;

void openPipes(BranchDescriptors *, int);

void closePipes(BranchDescriptors *, int, local_id);

void closeOtherChildDescriptors(BranchDescriptors *, local_id id, int);

void closeOtherParentDescriptors(BranchDescriptors *, int);

#endif //LAB1_DESCRIPTORS_H
