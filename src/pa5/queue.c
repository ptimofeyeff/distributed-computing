#include "queue.h"
#include <stdbool.h>
#include <stdio.h>

Queue queue;

bool isFull() {
    return queue.length == MAX_QUEUE_SIZE;
}

bool isEmpty() {
    return queue.length == 0;
}

// если первый больше второго возвращает 1, если второй больше первого возвращает -1
int compare(Request request1, Request request2) {
    if (request1.time < request2.time) {
        return 1;
    }
    if (request1.time == request2.time) {
        if (request1.procId < request2.procId) {
            return 1;
        } else if (request1.procId == request2.procId) {
            return 0;
        }
    }
    return -1;
}

// находит id максимального элемента в очереди по компаратору
int getMaxRequestId() {
    if (queue.length <= 0) {
        return -1;
    } else {
        int getMaxRequestId = 0;
        for (int i = 1; i < queue.length; ++i) {
            if (compare(queue.requests[i], queue.requests[getMaxRequestId]) > 0) { // если первый больше,
                getMaxRequestId = i;
            }
        }
        return getMaxRequestId;
    }
}

bool enqueue(Request request) {
    if (!isFull()) {
        queue.requests[queue.length] = request;
        queue.length++;
        return true;
    } else {
        printf("attempt to enqueue to full queue\n");
        return false;
    }
}

// вытаскивает первый элемент из очереди
Request dequeue() {
    if (!isEmpty()) {
        local_id maxRequestId = getMaxRequestId();
        Request maxRequest = queue.requests[maxRequestId];
        queue.requests[maxRequestId] = queue.requests[queue.length - 1];
        queue.length--;
        return maxRequest;
    } else {
        printf("attempt to dequeue from empty queue\n");
        Request request = { -1, -1};
        return request;
    }
}

// возвраает первый элемент их очереди без его удаления из очереди
Request peek() {
    if (!isEmpty()) {
        int maxRequestId = getMaxRequestId();
        return queue.requests[maxRequestId];
    } else {
        printf("attempt to pick from empty queue\n");
        Request request = { -1, -1};
        return request;
    }
}










