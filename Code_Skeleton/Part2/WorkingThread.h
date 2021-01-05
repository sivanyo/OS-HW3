//
// Created by 1912m on 01/01/2021.
//

#ifndef HW3_WORKINGTHREAD_H
#define HW3_WORKINGTHREAD_H


#include "Thread.hpp"
#include "Game.hpp"

struct cellNeighbors {
    int numAlive;
    vector<int> neighborConc;
};

class WorkingThread : public Thread {
private:
    int currPhase;

    // Reference to job queue
    PCQueue<TileJob>* jobQueue;
    // Reference to the vector containing the timings of all threads
    vector<double>* tileHist;
    // Reference to shared lock in order to allow atomic time update and counter raise
    pthread_mutex_t* threadLock;
    uint* finishedJobCounter;


protected:
    void thread_workload();

    static bool is_legal_neighbor(int i, int j, int height, int width);

    static cellNeighbors calculate_neighbors(int_mat* field, int i, int j, int height, int width);

    static int find_dominant_species(cellNeighbors env);

    int change_species_from_neighbors(cellNeighbors env, int selfVal);

public:
    WorkingThread(uint thread_id, PCQueue<TileJob>* jobQueue, vector<double>* tileHist, pthread_mutex_t* threadLock, uint* finishedJobCounter);

    ~WorkingThread() = default;

    void do_phase1(TileJob job);

    void do_phase2(TileJob job);
};


#endif //HW3_WORKINGTHREAD_H
