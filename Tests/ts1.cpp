#include "../Code_Skeleton/Part1/Headers.hpp"
#include "../Code_Skeleton/Part1/Semaphore.hpp"


// Threads & Synchronization
#include <pthread.h>

#define MAX_THREAD 100
#define MAX_THREAD_EXTRA 5000
#define MAX_CORES 5


Semaphore sem = Semaphore(MAX_CORES);
Semaphore sem1 = Semaphore(0);
int global_tmp = 1;

void* d1(void* arg)
{
    //cout<< "Entered d1" << endl;
    //wait
    sem1.down();
    cout<< "Entered.." << endl;

    //critical section
    sleep(1);
    cout << "Do something for the " << global_tmp++ << " time" << endl;

    //signal
    cout<< "Just Exiting.." << endl;
    sem1.up();
}

void* d2(void* arg)
{
    sem1.down();
    cout << "..." << global_tmp++ << "..." << endl;
    sem1.up();
}

void* f1(void* arg)
{
    //wait
    sem.down();
    int tmp = global_tmp++;
    cout << "Entered... curr id is " << tmp << endl;

    //critical section
//    sleep(2);

    //signal
    cout << "Just Exiting... my id was " << tmp << endl;
    sem.up();
}

void* f2(void* arg)
{
    sem.down();
    cout << "..." << global_tmp++ << "..." << endl;
    sem.up();
}

void* f3(void* arg)
{
    sem.down();
    cout << "..." << global_tmp++ << "..." << endl;
    sleep(4);
    sem.up();
}

int main(){

    sem1.up();
    cout << "***************************** test 1 *****************************" << endl;
    int tg_size = 10;
    pthread_t tg0 [tg_size];

    for(int i = 0; i < tg_size; ++i) { // start all threads to begin work
        pthread_create(&tg0[i], nullptr, d1, nullptr);
    }
    for(int i = 0; i < tg_size; ++i) { // wait for all threads to finish
        pthread_join(tg0[i], nullptr);
    }



    sleep(2);
    cout << "\n***************************** test 2 *****************************" << endl;
    global_tmp = 0;

    pthread_t thread_group [MAX_THREAD];

    for(int i = 0; i < MAX_THREAD; ++i) { // start all threads to begin work
        pthread_create(&thread_group[i], nullptr, f1, nullptr);
    }

    for(int i = 0; i < MAX_THREAD; ++i) { // wait for all threads to finish
        pthread_join(thread_group[i], nullptr);
    }

    sleep(2);
    cout << "\n***************************** test 3 *****************************" << endl;
    cout << "********************* test multi thread running ******************" << endl;
    global_tmp = 0;

    pthread_t tp2 [MAX_THREAD_EXTRA];

    for(int i = 0; i < MAX_THREAD_EXTRA; ++i) { // start all threads to begin work
        pthread_create(&tp2[i], nullptr, f2, nullptr);
    }

    for(int i = 0; i < MAX_THREAD_EXTRA; ++i) { // wait for all threads to finish
        pthread_join(tp2[i], nullptr);
    }


    sleep(2);

    cout << "\n***************************** test 4 *****************************" << endl;
    cout << "********************* test multi thread running ******************" << endl;
    global_tmp = 1;

    pthread_t tp3 [50];

    for(int i = 0; i < 50; ++i) { // start all threads to begin work
        pthread_create(&tp3[i], nullptr, f3, nullptr);
    }

    for(int i = 0; i < 50; ++i) { // wait for all threads to finish
        pthread_join(tp3[i], nullptr);
    }


    sleep(2);

    cout << "\n***************************** test 5 (d2) *****************************" << endl;
    cout << "********************* test multi thread running ******************" << endl;
    global_tmp = 1;

    pthread_t tp4 [MAX_THREAD_EXTRA];

    for(int i = 0; i < MAX_THREAD_EXTRA; ++i) {
        pthread_create(&tp4[i], nullptr, d2, nullptr);
    }

    for(int i = 0; i < MAX_THREAD_EXTRA; ++i) {
        pthread_join(tp4[i], nullptr);
    }

    return 0;


}

