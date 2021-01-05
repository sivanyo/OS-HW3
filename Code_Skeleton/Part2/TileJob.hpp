//
// Created by 1912m on 05/01/2021.
//

#include "../Part1/Headers.hpp"

struct TileTime {
    double threadTime;
    uint threadId;
};

struct TileJob {
    uint startRow;
    uint endRow;

    uint height;
    uint width;

    uint currPhase;
    int_mat *current;
    int_mat *next;

    bool isLastGen;
};