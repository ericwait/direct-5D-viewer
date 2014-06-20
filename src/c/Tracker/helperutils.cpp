#include "tracker.h"
#include "Hull.h"
#include "omp.h"

#include <set>

double calcCOMDist(int startCellIdx, int nextCellIdx)
{
	return gHulls[startCellIdx].getCenterOfMass().EuclideanDistanceTo(gHulls[nextCellIdx].getCenterOfMass());
}

double calcMinPixelDist(int startIdx, int nextIdx)
{
	double minDist = dbltype::infinity();
	for ( int i=0; i < gHulls[startIdx].getNumberofVoxels(); ++i )
	{
		for ( int j=0; j < gHulls[nextIdx].getNumberofVoxels(); ++j )
		{
			double dist = gHulls[startIdx].getNthVoxel(i).SquareDistanceTo(gHulls[nextIdx].getNthVoxel(j));

			if ( dist < minDist )
				minDist = dist;
		}
	}

	return sqrt(minDist);
}

void buildConnectedComponentDist(int maxLookahead)
{
	std::map<int,double>* cCDistMap = new std::map<int,double>[gHulls.size()];

	#pragma omp parallel for default(none) shared(maxLookahead,gHulls,gHashedHulls,cCDistMap,gVMax)
	for ( int i=0; i < gHulls.size(); ++i )
	{
		int t = gHulls[i].getFrame();
		for ( int tInc=1; tInc <= maxLookahead; ++tInc )
		{
			int tNext = t + tInc;

			if ( tNext >= gHashedHulls.size() )
				continue;
			
			std::set<int>::iterator nextIter = gHashedHulls[tNext].begin();
			for ( ; nextIter != gHashedHulls[tNext].end(); ++nextIter )
			{
				if ( calcCOMDist(i,(*nextIter)) > gVMax )
					continue;

				double ccDist = calcMinPixelDist(i, *nextIter);
				cCDistMap[i].insert(std::pair<int,double>(*nextIter,ccDist));
			}
		}
	}

	gTrackerData.setCCdistMap(cCDistMap);
}

void updateConnectedComponentDist(std::set<int>& hullList, bool bBidirectional)
{
	//TODO Stub Function
}