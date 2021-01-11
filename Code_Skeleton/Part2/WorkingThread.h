//
// Created by 1912m on 01/01/2021.
//

#ifndef HW3_WORKINGTHREAD_H
#define HW3_WORKINGTHREAD_H


#include "Thread.hpp"
#include "Game.hpp"

#define N 8

struct cellNeighbors {
    int numAlive = 0;
    int neighborConc[N] = {0,0,0,0,0,0,0,0};
};

class WorkingThread : public Thread {
private:
    // Reference to job queue
    PCQueue<TileJob>* jobQueue;
    // Reference to the vector containing the timings of all threads
    vector<TileTime>* tileHist;
    // Reference to shared lock in order to allow atomic time update and counter raise
    pthread_mutex_t* threadLock;
    uint* finishedJobCounter;


protected:
    void thread_workload();

    static cellNeighbors calculate_neighbors(int_mat* field, int i, int j, int height, int end, int left, int width);

    static int find_dominant_species(cellNeighbors env);

    int change_species_from_neighbors(cellNeighbors env, int selfVal);

public:
    WorkingThread(uint thread_id, PCQueue<TileJob>* jobQueue, vector<TileTime>* tileHist, pthread_mutex_t* threadLock, uint* finishedJobCounter);

    ~WorkingThread() = default;

    void do_phase1(TileJob job);

    void do_phase2(TileJob job);

    int generate_start_index(int i);

    int generate_end_index(int i, int height);

    int generate_left_index(int j);

    int generate_right_index(int j, int width);
};


#endif //HW3_WORKINGTHREAD_H
