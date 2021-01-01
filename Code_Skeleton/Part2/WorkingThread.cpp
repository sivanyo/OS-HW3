//
// Created by 1912m on 01/01/2021.
//

#include "WorkingThread.h"

WorkingThread::WorkingThread(uint thread_id, field_mat *current, field_mat *next, uint startRow, uint endRow, uint height,
                             uint width) : Thread(thread_id), current(current), next(next), startRow(startRow), endRow(endRow), height(height),
                                           width(width) {

}

cellNeighbors WorkingThread::calculate_neighbors(field_mat *field, int i, int j) {
    int height = (*field).size();
    int width = field[0].size();
    cellNeighbors env;
    env.numAlive = 0;
    for (int k = 0; k < 7; ++k) {
        env.neighborConc.push_back(0);
    }
    for (int k = i - 1; k <= i + 1; ++k) {
        for (int l = j - 1; l <= j + 1; ++l) {
            if (k == i && l == j) {
                continue;
            }
            if (is_legal_neighbor(k, l, height, width) && (*field)[k][l] != 0) {
                env.numAlive++;
                env.neighborConc[(*field)[k][l] % 7]++;
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

    // 1 is red
    // 0 is black (it's actually 7)
    for (int i = 1; i < env.neighborConc.size(); ++i) {
        int tempVal = env.neighborConc[i] * i;
        if (tempVal > maxVal) {
            maxVal = tempVal;
            maxIndex = i;
        }
    }

    int lastVal = env.neighborConc[0] * 7;
    if (lastVal > maxVal) {
        maxIndex = 7;
    }
    return maxIndex;
}

int WorkingThread::change_species_from_neighbors(cellNeighbors env, int selfVal) {
    int sum = selfVal;
    int counter = 1;
    for (int i = 1; i < env.neighborConc.size(); ++i) {
        if (env.neighborConc[i] != 0) {
            sum += env.neighborConc[i] * i;
            counter += env.neighborConc[i];
        }
    }

    if (env.neighborConc[0] != 0) {
        sum += env.neighborConc[0] * 7;
        counter += env.neighborConc[0];
    }
    return std::round(sum / counter);
}

bool WorkingThread::is_legal_neighbor(int i, int j, int height, int width) {
    return i >= 0 && i < height && j >= 0 && j < width;
}