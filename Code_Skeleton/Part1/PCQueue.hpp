#ifndef _QUEUEL_H
#define _QUEUEL_H

#include "Headers.hpp"

// Single Producer - Multiple Consumer itemQueue
template<typename T>
class PCQueue {

public:
    PCQueue();

    ~PCQueue();

    // Blocks while itemQueue is empty. When itemQueue holds items, allows for a single
    // thread to enter and remove an item from the front of the itemQueue and return it.
    // Assumes multiple consumers.
    T pop();

    // Allows for producer to enter with *minimal delay* and push items to back of the itemQueue.
    // Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.
    // Assumes single producer
    void push(const T &item);


private:
    int producersInside = 0;
    int producersWaiting = 0;
    int consumersInside = 0;
    pthread_mutex_t mutex;
    pthread_cond_t allowConsume;
    pthread_cond_t allowProduce;
    queue<T> itemQueue;
    int queueSize = 0;

    void consumerLock();

    void consumerUnlock();

    void producerLock();

    void producerUnlock();
};

template<typename T>
PCQueue<T>::PCQueue(): itemQueue(), consumersInside(0), producersInside(0), producersWaiting(0) {
    pthread_mutexattr_t attribute;
    pthread_mutexattr_init(&attribute);
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attribute);
    pthread_cond_init(&allowConsume, nullptr);
    pthread_cond_init(&allowProduce, nullptr);
}

template<typename T>
PCQueue<T>::~PCQueue() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&allowConsume);
    pthread_cond_destroy(&allowProduce);
}

template<typename T>
T PCQueue<T>::pop() {
    std::cout << "received pop request with " << std::endl;
    consumerLock();
    while (queueSize == 0) {
        //pthread_cond_wait(&queueCond, &queueMutex);
    }
    queueSize--;
    // getting the first from itemQueue
    //std::cout << "queue size is now: " << itemQueue.size() << std::endl;
    std::cout << "internal queue size is now: " << queueSize << std::endl;
    T first = itemQueue.front();
    itemQueue.pop();
    consumerUnlock();
    return first;
}

template<typename T>
void PCQueue<T>::push(const T &item) {
    std::cout << "received push request with " << item << std::endl;
    producerLock();
    queueSize++;
    itemQueue.push(item);
    producerUnlock();
}

template<typename T>
void PCQueue<T>::consumerLock() {
    pthread_mutex_lock(&mutex);
    while (producersInside > 0 || producersWaiting > 0) {
        // We want to give priority to producers, so we are waiting for as long as there are producers working
        pthread_cond_wait(&allowConsume, &mutex);
    }
    // Consumer is now working
    consumersInside++;
    //pthread_mutex_unlock(&mutex);
}

template<typename T>
void PCQueue<T>::consumerUnlock() {
    //pthread_mutex_lock(&mutex);
    consumersInside--;
    if (consumersInside == 0) {
        // There are no more consumers, so we can let the producer enter
        pthread_cond_signal(&allowProduce);
    }
    pthread_mutex_unlock(&mutex);
}

template<typename T>
void PCQueue<T>::producerLock() {
    pthread_mutex_lock(&mutex);
    producersWaiting++;
    while (producersInside + consumersInside > 0) {
        // We want to wait until no one is working on the itemQueue
        pthread_cond_wait(&allowProduce, &mutex);
    }
    producersWaiting--;
    producersInside++;
    //pthread_mutex_unlock(&mutex);
}

template<typename T>
void PCQueue<T>::producerUnlock() {
    //pthread_mutex_lock(&mutex);
    producersInside--;
    if (producersInside == 0) {
        // There are no more producers, we want to give priority to the next producer if there is one, and then send a message
        // to all consumers that they can begin working on the itemQueue
        pthread_cond_signal(&allowProduce);
        pthread_cond_broadcast(&allowConsume);
    }
    pthread_mutex_unlock(&mutex);
}

#endif