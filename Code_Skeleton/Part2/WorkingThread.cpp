//
// Created by 1912m on 01/01/2021.
//

#include "WorkingThread.h"

WorkingThread::WorkingThread(uint thread_id, PCQueue<TileJob> *jobQueue, vector<TileTime> *tileHist, pthread_mutex_t *threadLock,
                             uint *finishedJobCounter) : Thread(thread_id), jobQueue(jobQueue), tileHist(tileHist),
                                                         threadLock(threadLock), finishedJobCounter(finishedJobCounter) {

}

cellNeighbors WorkingThread::calculate_neighbors(int_mat *field, int i, int j, int start, int end, int left, int right) {
    cellNeighbors env;
    env.numAlive = 0;
    for (int k = start; k <= end; ++k) {
        for (int l = left; l <= right; ++l) {
            if (k == i && l == j) {
                continue;
            }
            if ((*field)[k][l] != 0) {
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

    for (unsigned int i = 1; i < 8; ++i) {
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
    for (unsigned int i = 1; i < 8; ++i) {
        if (env.neighborConc[i] != 0) {
            sum += env.neighborConc[i] * i;
            counter += env.neighborConc[i];
        }
    }
    double res = (double) sum / counter;
    return std::round(res);
}

void WorkingThread::thread_workload() {
    bool isFinished = false;
    while (true) {
        TileJob job = jobQueue->pop();
        isFinished = job.isLastGen;
        if (isFinished) {
            return;
        }

        auto startMeasure = std::chrono::system_clock::now();
        if (job.currPhase == 1) {
            do_phase1(job);
        } else {
            do_phase2(job);
        }
        auto endMeasure = std::chrono::system_clock::now();
        TileTime tt{};
        tt.threadId = m_thread_id;
        tt.threadTime = (double) std::chrono::duration_cast<std::chrono::microseconds>(endMeasure - startMeasure).count();
        pthread_mutex_lock(threadLock);
        tileHist->push_back(tt);
        (*finishedJobCounter)++;
        pthread_mutex_unlock(threadLock);
    }
}

void WorkingThread::do_phase1(TileJob job) {
    // PHASE 1
    for (int i = (int) job.startRow; i <= (int) job.endRow; ++i) {
        int start = generate_start_index(i);
        int end = generate_end_index(i, job.height);
        for (int j = 0; j < (int) job.width; ++j) {
            int left = generate_left_index(j);
            int right = generate_right_index(j, job.width);
            cellNeighbors env = calculate_neighbors(job.current, i, j, start, end, left, right);
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
    for (int i = job.startRow; i <= (int) job.endRow; ++i) {
        int start = generate_start_index(i);
        int end = generate_end_index(i, job.height);
        for (int j = 0; j < (int) job.width; ++j) {
            int left = generate_left_index(j);
            int right = generate_right_index(j, job.width);
            if ((*job.next)[i][j] != 0) {
                // this cell is alive and needs to be updated
                cellNeighbors env = calculate_neighbors(job.next, i, j, start, end, left, right);
                (*job.current)[i][j] = change_species_from_neighbors(env, (*job.next)[i][j]);
            } else {
                (*job.current)[i][j] = 0;
            }
        }
    }
}

int WorkingThread::generate_start_index(int i) {
    if (0 > i - 1) {
        return 0;
    } else {
        return i - 1;
    }
}

int WorkingThread::generate_end_index(int i, int height) {
    if (i + 1 < height) {
        return i + 1;
    } else {
        return height - 1;
    }
}

int WorkingThread::generate_left_index(int j) {
    if (0 > j - 1) {
        return 0;
    } else {
        return j - 1;
    }
}

int WorkingThread::generate_right_index(int j, int width) {
    if (width - 1 < j + 1) {
        return width - 1;
    } else {
        return j + 1;
    }
}