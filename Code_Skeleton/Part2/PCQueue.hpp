#ifndef _QUEUEL_H
#define _QUEUEL_H

#include "Headers.hpp"
#include "Semaphore.hpp"

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
    pthread_mutex_t mutex;
    queue<T> itemQueue;
    Semaphore availItems;
    int numOfProducersWaiting = 0;
};

template<typename T>
PCQueue<T>::PCQueue(): itemQueue(), availItems(0) {
    pthread_mutex_init(&mutex, NULL);
}

template<typename T>
PCQueue<T>::~PCQueue() {
    pthread_mutex_destroy(&mutex);
}

template<typename T>
T PCQueue<T>::pop() {
    availItems.down();
    while (numOfProducersWaiting > 0) {
        sched_yield();
    }
    pthread_mutex_lock(&mutex);
    T first = itemQueue.front();
    itemQueue.pop();
    pthread_mutex_unlock(&mutex);
    return first;
}

template<typename T>
void PCQueue<T>::push(const T &item) {
    numOfProducersWaiting++;
    pthread_mutex_lock(&mutex);
    numOfProducersWaiting--;
    itemQueue.push(item);
    pthread_mutex_unlock(&mutex);
    availItems.up();
}

#endif