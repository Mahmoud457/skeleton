#ifndef HASHSET_H
#define HASHSET_H
#include <pthread.h>


typedef struct Node{
    char* ipAddress;
    struct Node* next;
} Node;

typedef struct HashSet {
    Node** elements;
    int maxCap;
    int numElements;
    float loadfactor;
    pthread_mutex_t lock; 
}HashSet;

HashSet* createHashSet(int maxCap, float loadfactor);
int addElement(HashSet* set, const char* ipAddress);
void resizeHashset(HashSet* hashset);
void destroyHashSet(HashSet* hashset);

#endif