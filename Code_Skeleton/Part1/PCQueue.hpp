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
    bool isProducerWaiting = false;
};

template<typename T>
PCQueue<T>::PCQueue(): itemQueue(), availItems(0) {
    pthread_mutexattr_t attribute;
    pthread_mutexattr_init(&attribute);
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attribute);
}

template<typename T>
PCQueue<T>::~PCQueue() {
    pthread_mutex_destroy(&mutex);
}

template<typename T>
T PCQueue<T>::pop() {
    while (isProducerWaiting) {
        //std::cout << "there are producers waiting to write, so I am giving up on CPU" << std::endl;
        sched_yield();
    }
    availItems.down();
    pthread_mutex_lock(&mutex);
    T first = itemQueue.front();
    itemQueue.pop();
    pthread_mutex_unlock(&mutex);
    return first;
}

template<typename T>
void PCQueue<T>::push(const T &item) {
    itemQueue.push(item);
    isProducerWaiting = true;
    availItems.up();
    isProducerWaiting = false;
}

#endif