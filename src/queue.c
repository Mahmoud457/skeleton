#include "queue.h"
#include <stdlib.h>

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->rear = NULL;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond, NULL);
    return queue;
}

void destroyQueue(Queue* queue) {
    pthread_mutex_lock(&queue->lock);
    QueueNode* current = queue->front;
    while (current) {
        QueueNode* temp = current;
        current = current->next;
        free(temp);
    }
    pthread_mutex_unlock(&queue->lock);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    free(queue);
}

void queueEnqueue(Queue* queue, void* data) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->data = data;
    newNode->next = NULL;

    pthread_mutex_lock(&queue->lock);
    if (queue->rear) {
        queue->rear->next = newNode;
    } else {
        queue->front = newNode;
    }
    queue->rear = newNode;
    pthread_cond_signal(&queue->cond); 
    pthread_mutex_unlock(&queue->lock);
}


void* queueDequeue(Queue* queue) {
    pthread_mutex_lock(&queue->lock);

    while (queue->front == NULL) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }

    if (queue->front == NULL) {
        pthread_mutex_unlock(&queue->lock);
        return NULL; 
    }

    QueueNode* frontNode = queue->front;
    void* data = frontNode->data;
    queue->front = frontNode->next;

    if (queue->front == NULL) {
        queue->rear = NULL; 
    }

    free(frontNode);
    pthread_mutex_unlock(&queue->lock);

    return data;
}
int queueIsEmpty(Queue* queue) {
    pthread_mutex_lock(&queue->lock); 
    int isEmpty = (queue->front == NULL); 
    pthread_mutex_unlock(&queue->lock); 
    return isEmpty; 
}




