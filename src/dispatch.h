#ifndef CS241_DISPATCH_H
#define CS241_DISPATCH_H

#include "hashset.h"
#include <pcap.h>
#include <stdatomic.h>
#include "queue.h"

typedef struct Task{
    struct pcap_pkthdr* header;
    const unsigned char* packet;
    int verbose;
    HashSet* synIps;
    atomic_int* synCount;
    atomic_int* ArpCount;
    atomic_int* googleCount;
    atomic_int* bbcCount;

}Task;

typedef struct ThreadPool{
    pthread_t* threads;
    int threadCount;
    Queue* taskQueue;
    int stop;
    pthread_mutex_t lock;
    pthread_cond_t cond;

}ThreadPool;


void initialiseThreadPool(int thread_count);

void dispatch(struct pcap_pkthdr *header, 
              const unsigned char *packet,
              int verbose, HashSet* synIps, atomic_int* synCount, atomic_int* ArpCount, atomic_int* googleCount, atomic_int* bbcCount);

void cleanupThreadPool();

#endif
