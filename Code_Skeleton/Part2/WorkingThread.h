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

    uint startRow;
    uint endRow;

    uint height;
    uint width;

    field_mat *current;
    field_mat *next;
    vector<float>* m_tile_hist;

//    pthread_mutex_t* lock;

protected:
    void thread_workload();

    static bool is_legal_neighbor(int i, int j, int height, int width);

    static cellNeighbors calculate_neighbors(field_mat* field, int i, int j, int height, int width);

    static int find_dominant_species(cellNeighbors env);

    int change_species_from_neighbors(cellNeighbors env, int selfVal);

public:
//    WorkingThread(uint thread_id, field_mat *current, field_mat *next, uint startRow, uint endRow, uint height,
//                  uint width, vector<float>* m_tile_hist, pthread_mutex_t* lock);
    WorkingThread(uint thread_id, field_mat *current, field_mat *next, uint startRow, uint endRow, uint height,
                  uint width, vector<float>* m_tile_hist);

    ~WorkingThread() = default;

    void do_phase1();

    void do_phase2();
};


#endif //HW3_WORKINGTHREAD_H
