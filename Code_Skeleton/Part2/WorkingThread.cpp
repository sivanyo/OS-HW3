//
// Created by 1912m on 01/01/2021.
//

#include "WorkingThread.h"

WorkingThread::WorkingThread(uint thread_id, PCQueue<TileJob> *jobQueue, vector<TileTime> *tileHist, pthread_mutex_t *threadLock,
                             uint *finishedJobCounter) : Thread(thread_id), currPhase(1), jobQueue(jobQueue), tileHist(tileHist),
                                                         threadLock(threadLock), finishedJobCounter(finishedJobCounter) {

}

cellNeighbors WorkingThread::calculate_neighbors(int_mat *field, int i, int j, int height, int width) {
    cellNeighbors env;
    env.numAlive = 0;
    for (unsigned int k = 0; k < 8; ++k) {
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
    bool isFinished = false;
    while (isFinished == false) {
        TileJob job = jobQueue->pop();
        isFinished = job.isLastGen;
        if (isFinished) {
            return;
        }

        auto startMeasure = std::chrono::system_clock::now();
        if (currPhase == 1) {
            do_phase1(job);
            currPhase = 2;
        } else {
            do_phase2(job);
            currPhase = 1;
        }
        auto endMeasure = std::chrono::system_clock::now();
        TileTime tt{};
        tt.threadId = m_thread_id;
        tt.threadTime = (double) std::chrono::duration_cast<std::chrono::microseconds>(endMeasure - startMeasure).count();
        pthread_mutex_lock(threadLock);
        (*finishedJobCounter)++;
        tileHist->push_back(tt);
        pthread_mutex_unlock(threadLock);
    }

}

void WorkingThread::do_phase1(TileJob job) {
    // PHASE 1
    for (unsigned int i = job.startRow; i <= job.endRow; ++i) {
        for (unsigned int j = 0; j < job.width; ++j) {
            cellNeighbors env = calculate_neighbors(job.current, i, j, job.height, job.width);
            if ((*job.current)[i][j] != 0) {
                // this cell is alive
                if (env.numAlive <= 1 || env.numAlive > 3) {
                    // this cell needs to be killed
                    (*job.next)[i][j] = 0;
                } else {
                    // this cell remains the same
                    (*job.next)[i][j] = (*job.current)[i][j];
                }
            } else {
                // this cell is dead
                if (env.numAlive == 3) {
                    // this cell will now be born
                    (*job.next)[i][j] = find_dominant_species(env);
                } else {
                    // this cell will not be born
                    (*job.next)[i][j] = (*job.current)[i][j];
                }
            }
        }
    }
}

void WorkingThread::do_phase2(TileJob job) {
    // PHASE 2
    for (unsigned int i = job.startRow; i <= job.endRow; ++i) {
        for (unsigned int j = 0; j < job.width; ++j) {
            if ((*job.next)[i][j] != 0) {
                // this cell is alive and needs to be updated
                cellNeighbors env = calculate_neighbors(job.next, i, j, job.height, job.width);
                (*job.current)[i][j] = change_species_from_neighbors(env, (*job.next)[i][j]);
            } else {
                (*job.current)[i][j] = 0;
            }
        }
    }
}
