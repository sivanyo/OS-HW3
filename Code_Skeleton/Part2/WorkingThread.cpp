//
// Created by 1912m on 01/01/2021.
//

#include "WorkingThread.h"

WorkingThread::WorkingThread(uint thread_id, field_mat *current, field_mat *next, uint startRow, uint endRow, uint height,
                             uint width, vector<float> *m_tile_hist) : Thread(thread_id), currPhase(1), startRow(startRow), endRow(endRow),
                                                                       height(height), width(width), current(current), next(next),
                                                                       m_tile_hist(m_tile_hist) {

}

//WorkingThread::WorkingThread(uint thread_id, field_mat *current, field_mat *next, uint startRow, uint endRow, uint height,
//                             uint width, vector<float> *m_tile_hist, pthread_mutex_t* lock) : Thread(thread_id), currPhase(1), startRow(startRow), endRow(endRow),
//                                                                                              height(height), width(width), current(current), next(next),
//                                                                                              m_tile_hist(m_tile_hist), lock(lock) {
//
//}

cellNeighbors WorkingThread::calculate_neighbors(field_mat *field, int i, int j, int height, int width) {
    cellNeighbors env;
    env.numAlive = 0;
    for (int k = 0; k < 8; ++k) {
        env.neighborConc.push_back(0);
    }
    for (int k = i - 1; k <= i + 1; ++k) {
        for (int l = j - 1; l <= j + 1; ++l) {
            if (k == i && l == j) {
                continue;
            }
            if (is_legal_neighbor(k, l, height, width) && (*field)[k][l] != 0) {
                env.numAlive++;
                env.neighborConc[(*field)[k][l]]++;
            }
        }
    }
    return env;
}

// This function will return the index of the most dominant species in the
// neighborhood
int WorkingThread::find_dominant_species(cellNeighbors env) {
    int maxIndex = -1;
    int maxVal = 0;

    for (unsigned int i = 1; i < env.neighborConc.size(); ++i) {
        int tempVal = env.neighborConc[i] * i;
        if (tempVal > maxVal) {
            maxVal = tempVal;
            maxIndex = i;
        }
    }

    return maxIndex;
}

int WorkingThread::change_species_from_neighbors(cellNeighbors env, int selfVal) {
    int sum = selfVal;
    int counter = 1;
    for (unsigned int i = 1; i < env.neighborConc.size(); ++i) {
        if (env.neighborConc[i] != 0) {
            sum += env.neighborConc[i] * i;
            counter += env.neighborConc[i];
        }
    }
    double res = (double) sum / counter;
    return std::round(res);
}

bool WorkingThread::is_legal_neighbor(int i, int j, int height, int width) {
    return i >= 0 && i < height && j >= 0 && j < width;
}

void WorkingThread::thread_workload() {

    if (currPhase == 1) {
//        auto measureStart = std::chrono::system_clock::now();
        do_phase1();
        currPhase = 2;
//        auto measureEnd = std::chrono::system_clock::now();
//        timing = (float) std::chrono::duration_cast<std::chrono::microseconds>(measureEnd - measureStart).count();
    } else {
//        auto measureStart = std::chrono::system_clock::now();
        do_phase2();
        currPhase = 1;
//        auto measureEnd = std::chrono::system_clock::now();
//        timing += (float) std::chrono::duration_cast<std::chrono::microseconds>(measureEnd - measureStart).count();
    }


//    pthread_mutex_lock(lock);
//    (*m_tile_hist).push_back((float) std::chrono::duration_cast<std::chrono::microseconds>(measureEnd - measureStart).count());
//    pthread_mutex_unlock(lock);
}

void WorkingThread::do_phase1() {
    // PHASE 1
    for (unsigned int i = startRow; i <= endRow; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            cellNeighbors env = calculate_neighbors(current, i, j, height, width);
            if ((*current)[i][j] != 0) {
                // this cell is alive
                if (env.numAlive <= 1 || env.numAlive > 3) {
                    // this cell needs to be killed
                    (*next)[i][j] = 0;
                } else {
                    // this cell remains the same
                    (*next)[i][j] = (*current)[i][j];
                }
            } else {
                // this cell is dead
                if (env.numAlive == 3) {
                    // this cell will now be born
                    (*next)[i][j] = find_dominant_species(env);
                } else {
                    // this cell will not be born
                    (*next)[i][j] = (*current)[i][j];
                }
            }
        }
    }
}

void WorkingThread::do_phase2() {
    // PHASE 2
    for (unsigned int i = startRow; i <= endRow; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            if ((*next)[i][j] != 0) {
                // this cell is alive and needs to be updated
                cellNeighbors env = calculate_neighbors(next, i, j, height, width);
                (*current)[i][j] = change_species_from_neighbors(env, (*next)[i][j]);
            } else {
                (*current)[i][j] = 0;
            }
        }
    }
}
