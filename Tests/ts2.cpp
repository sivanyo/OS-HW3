// Threads & Synchronization
#include <pthread.h>
#include "../Code_Skeleton/Part1/PCQueue.hpp"

#define MAX_THREAD 100
#define MAX_THREAD_EXTRA 5000
#define MAX_CORES 5

PCQueue<int> m_q = PCQueue<int>();

void Test1 (){
    PCQueue<int> m_q = PCQueue<int>();
    m_q.push(5);
    int x = m_q.pop();
    assert(x==5);
}


void Test2 (){

    int x = m_q.pop();
    m_q.push(5);
    assert(x==5);
}

void Test3 (){
    PCQueue<int> m_q = PCQueue<int>();
    int x = m_q.pop();
    int y = m_q.pop();
    m_q.push(5);
    m_q.push(4);
    assert(x==5);
    assert(y==4);
}

void* push(void* arg){
    int* x = static_cast<int*>(arg);
    m_q.push(*x);
}

void* pop(void* arg) {
    int* x = static_cast<int*>(arg);
    *x = m_q.pop();
}

int main(){

    cout << "***************************** test 1 *****************************" << endl;
    pthread_t tg0, tg1,tg2, tg3;
    int x1,x2;
    int y1=1;
    int y2=2;
    pthread_create(&tg0, nullptr, pop, &x1);
    pthread_create(&tg0, nullptr, pop, &x2);
    pthread_create(&tg1, nullptr, push, &y1);
    sleep(2);
    pthread_create(&tg1, nullptr, push, &y2);
    sleep(5);
    std::cout << "x1 is: " << x1 << std::endl;
    std::cout << "x2 is: " << x2 << std::endl;
    assert(x1 == 1);
    assert(x2 == 2);

}