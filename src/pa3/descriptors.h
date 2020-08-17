#ifndef LAB1_DESCRIPTORS_H
#define LAB1_DESCRIPTORS_H

#include "ipc.h"

#define DESC_COUNT 2
#define READ_DESC 0
#define WRITE_DESC 1
#define MAX_PROC 11

typedef struct {
    int descriptors[MAX_PROC][MAX_PROC][DESC_COUNT];
} TopologyDescriptors;


void openPipes(TopologyDescriptors *, int);

void closePipes(TopologyDescriptors *, int, local_id);

void closeOtherChildDescriptors(TopologyDescriptors *, local_id id, int);

void closeOtherParentDescriptors(TopologyDescriptors *, int);

#endif //LAB1_DESCRIPTORS_H
