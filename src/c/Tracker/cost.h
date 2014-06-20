#ifndef COSTS_H
#define COSTS_H

#include <vector>

// Get cost based on a frame and index list.  The srcFrameIdx is used if there, it is the index into
// frame/index vectors of the source point(start of new path).  srcFrameIdx is trivially 0 if there is
// no history being used.
double getCost(std::vector<int>& indices, int sourceIdx, int bCheck = 1);

#endif