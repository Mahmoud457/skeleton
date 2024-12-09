#include "dispatch.h"
#include <pcap.h>
#include "analysis.h"
#include "hashset.h"
#include <pthread.h>
#include "queue.h"
#include <stdlib.h> 

static ThreadPool* pool = NULL;


void* threadWorker(void* tPool) {
    ThreadPool* pool = (ThreadPool*)tPool;

    while (1) {
        pthread_mutex_lock(&pool->lock);

        while (!pool->stop && queueIsEmpty(pool->taskQueue)) {
            pthread_cond_wait(&pool->cond, &pool->lock);
        }

        if (pool->stop) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        Task* task = (Task*)queueDequeue(pool->taskQueue);
        pthread_mutex_unlock(&pool->lock);

        if (task) {
            analyse(task->header, task->packet, task->verbose, task->synIps,
                    task->synCount, task->ArpCount, task->googleCount, task->bbcCount);
            free(task);
        }
    }

    return NULL;
}

void initialiseThreadPool(int numThreads){
    pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    pool->threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));
    pool->taskQueue = createQueue();
    pool->stop = 0;
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);
    pool->threadCount = numThreads;

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&pool->threads[i], NULL, threadWorker, pool);
    }
}

void dispatch(struct pcap_pkthdr *header,
              const unsigned char *packet,
              int verbose, HashSet* synIps, atomic_int* synCount, atomic_int* ArpCount, atomic_int* googleCount, atomic_int* bbcCount) {


    Task* task = (Task*)malloc(sizeof(Task));
    task->header = header;
    task->packet = packet;
    task->verbose = verbose;
    task->synIps = synIps;
    task->synCount = synCount;
    task->ArpCount = ArpCount;
    task->googleCount = googleCount;
    task->bbcCount = bbcCount;

    pthread_mutex_lock(&pool->lock);
    queueEnqueue(pool->taskQueue, task);
    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->lock);

  
  
}
void cleanupThreadPool(){
    pthread_mutex_lock(&pool->lock);
    pool->stop = 1;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->lock);

    for (int i = 0; i < pool->threadCount; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    destroyQueue(pool->taskQueue);
    free(pool->threads);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    free(pool);
    pool = NULL;

}
