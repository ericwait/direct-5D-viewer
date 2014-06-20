#ifndef HELPERUTILS_H
#define HELPERUTILS_H

#include <vector>

// Create the connected-component distances in gCCDistMap
void buildConnectedComponentDist(int maxLookahead);

// Update the connected-component distances in gCCDistMap
void updateConnectedComponentDist(std::vector<int>& hullList, bool bBidirectional = 0);

#endif