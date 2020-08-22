#include "lamport.h"
#include "banking.h"

timestamp_t lamportTime = 0;

timestamp_t get_lamport_time() {
    return lamportTime;
}

void incrementLamportTime() {
    lamportTime++;
}

void setLamportTime(timestamp_t timestamp) {
    lamportTime = timestamp;
}
