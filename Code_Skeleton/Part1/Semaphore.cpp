//
// Created by 1912m on 30/12/2020.
//

#include "Semaphore.hpp"

// Constructs a new semaphore with a counter of 0
Semaphore::Semaphore(): counter(0) {
    pthread_mutexattr_t attribute;
    pthread_mutexattr_init(&attribute);
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attribute);
    pthread_cond_init(&cond, nullptr);
}

// Constructs a new semaphore with a counter of val
Semaphore::Semaphore(unsigned val): counter(val) {
    pthread_mutexattr_t attribute;
    pthread_mutexattr_init(&attribute);
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &attribute);
    pthread_cond_init(&cond, nullptr);
}

// Mark: 1 Thread has left the critical section
void Semaphore::up() {
    // a thread has left the critical section, so we can add to the counter, so other processes waiting can start doing work
    pthread_mutex_lock(&mutex);
    // now we have the lock, so we can change counter
    counter++;
    pthread_cond_signal(&cond);
    // we have finished the critical section, so we unlock the mutex
    pthread_mutex_unlock(&mutex);
}

// Block until counter >0, and mark - One thread has entered the critical section.
// Wait until there is an available thread, and then use an available thread for the critical section
void Semaphore::down() {
    pthread_mutex_lock(&mutex);
    while (counter == 0) {
        // wait until a thread is available, the wait will stop by sending a signal to cond
        pthread_cond_wait(&cond, &mutex);
    }
    // thread has become available, need to use a thread, and take out a request from the wait queue
    // now we have the lock, so we can change counter
    counter--;
    // we have finished the critical section, so we unlock the mutex
    pthread_mutex_unlock(&mutex);
}
