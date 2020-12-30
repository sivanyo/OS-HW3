// Threads & Synchronization
#include <pthread.h>
#include "../Code_Skeleton/Part1/PCQueue.cpp"

#define MAX_THREAD 100
#define MAX_THREAD_EXTRA 1000
#define MAX_CORES 5

PCQueue<int> m_q;


void* push(void* arg){
    int* x = static_cast<int*>(arg);
    m_q.push(*x);
}

void* pop(void* arg) {
    int* x = static_cast<int*>(arg);
    *x = m_q.pop();
}


void* fp1_in(void* arg){
    int* max = static_cast<int*>(arg);
    cout << "start pushing " << *max << " items" << endl;
    for (int i = 0; i < *max; ++i) {
        m_q.push(i);
    }
    cout << "finish pushing " << *max << " items" << endl;
}

void* fp1_out(void* arg){
    cout << m_q.pop() << " was taken out" << endl;
}

void test1 (int size) {
    cout << "***************************** start test 1 *****************************" << endl;
    int tg_size = size;
    pthread_t tg [tg_size];

    pthread_t pro;
    int tg_size_tmp = tg_size/2;
    pthread_create(&pro, nullptr, fp1_in, &tg_size_tmp);
    for(int i = 0; i < tg_size; ++i) { // start all threads to begin work
        pthread_create(&tg[i], nullptr, fp1_out, nullptr);
    }

    tg_size_tmp = tg_size - tg_size_tmp;
    pthread_create(&pro, nullptr, fp1_in, &tg_size_tmp);


    for(int i = 0; i < tg_size; ++i) { // wait for all threads to finish
        pthread_join(tg[i], nullptr);
    }
    pthread_join(pro, nullptr);

    cout << "***************************** finish test 1 *****************************" << endl;
}

void* fp2_in(void* arg){
    int* arr = static_cast<int*>(arg);
    cout << "start pushing nums from " << arr[1] << " to " << arr[0]-1 << endl;
    for (int i = arr[1]; i < arr[0]; ++i) {
        m_q.push(i);
    }
    cout << "finish pushing nums from " << arr[1] << " to " << arr[0]-1 << endl;
}

void test2 (int size) {
    cout << "***************************** start test 2 *****************************" << endl;
    int tg_size = size;
    pthread_t tg [tg_size];

    pthread_t pro;
    int tg_size_tmp = tg_size/3;
    int base = 0;
    pthread_create(&pro, nullptr, fp2_in, (&base, &tg_size_tmp));
    for(int i = 0; i < tg_size; ++i) { // start all threads to begin work
        pthread_create(&tg[i], nullptr, fp1_out, nullptr);
    }

    sleep(1);

    base = tg_size_tmp;
    tg_size_tmp += tg_size/3;
    pthread_create(&pro, nullptr, fp2_in,(&base, &tg_size_tmp));

    sleep(1);

    base = tg_size_tmp;
    tg_size_tmp = tg_size;
//    tg_size_tmp = tg_size - 1; // TODO: for testing: change to this line and the program will never end
    pthread_create(&pro, nullptr, fp2_in,(&base, &tg_size_tmp));


    for(int i = 0; i < tg_size; ++i) { // wait for all threads to finish
        pthread_join(tg[i], nullptr);
    }
    pthread_join(pro, nullptr);

    cout << "***************************** finish test 2 *****************************" << endl;
}

int main(){

    test1(MAX_THREAD_EXTRA);
    cout << endl;
    sleep(1);
    test2(MAX_THREAD_EXTRA);

}