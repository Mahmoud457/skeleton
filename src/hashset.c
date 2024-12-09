#include "hashset.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>




int hash(const char* ipAddress, int maxCap){
    int value = 0;

    for(int i =0; ipAddress[i] != '\0'; i++){
        value = (value*37) + ipAddress[i];
    }
    return value % maxCap;


}

HashSet* createHashSet(int initCap, float loadfactor){
    HashSet* hashset = (HashSet*)malloc(sizeof(HashSet));
    hashset->maxCap = initCap;
    hashset->numElements = 0;
    hashset->loadfactor = loadfactor;
    hashset->elements = (Node**)calloc(initCap, sizeof(Node*)); 
    pthread_mutex_init(&hashset->lock, NULL);
    return hashset;

}

void resizeHashset(HashSet* hashset){
    pthread_mutex_lock(&hashset->lock);
    int newCap = hashset -> maxCap * 2;
    Node** newElements = (Node**)calloc(newCap, sizeof(Node*));


    for (int j = 0; j< hashset -> maxCap; j++){
        Node* currNode = hashset -> elements[j];
        while(currNode){
            int index = hash(currNode->ipAddress, newCap);

            Node* next = currNode->next;
            currNode->next = newElements[index];
            newElements[index] = currNode;
            currNode = next;
            
        }
    }
    free(hashset->elements);
    hashset->elements = newElements;
    hashset->maxCap = newCap;
    pthread_mutex_unlock(&hashset->lock);
}

int addElement(HashSet* hashset, const char* ipAddress){
    pthread_mutex_lock(&hashset->lock);
    if((float)hashset->numElements / hashset->maxCap >= hashset->loadfactor){
        resizeHashset(hashset);
    }

    int index = hash(ipAddress, hashset->maxCap);

    Node* currNode = hashset->elements[index];

    while (currNode){
        if (strcmp(currNode->ipAddress, ipAddress) == 0){
            pthread_mutex_unlock(&hashset->lock);

            return 0;
        }
        currNode = currNode -> next;
    }

    Node* new = (Node*)malloc(sizeof(Node));
    new->ipAddress = strdup(ipAddress);

    new->next = hashset->elements[index];
    hashset->elements[index] = new;
    hashset->numElements++;
    pthread_mutex_unlock(&hashset->lock);

    return 1;

}
void destroyHashSet(HashSet* hashset) {
    if (!hashset) return;

    for (int i = 0; i < hashset->maxCap; i++) {
        Node* currNode = hashset->elements[i];
        while (currNode) {
            Node* temp = currNode;
            currNode = currNode->next;
            free(temp->ipAddress);
            free(temp);
        }
    }

    free(hashset->elements);
    pthread_mutex_destroy(&hashset->lock);
    free(hashset);
}



