#ifndef LAB1_LAMPORT_H
#define LAB1_LAMPORT_H
#include "ipc.h"

void incrementLamportTime();

void setLamportTime(timestamp_t);

timestamp_t get_lamport_time();

#endif //LAB1_LAMPORT_H
