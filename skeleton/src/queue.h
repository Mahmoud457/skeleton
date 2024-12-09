#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

typedef struct QueueNode {
    void* data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} Queue;

Queue* createQueue();
void destroyQueue(Queue* queue);
void queueEnqueue(Queue* queue, void* data);
void* queueDequeue(Queue* queue);
int queueIsEmpty(Queue* queue);
#endif