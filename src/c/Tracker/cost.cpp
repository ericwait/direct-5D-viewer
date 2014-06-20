#include "tracker.h"
#include "Utility.h"
#include "Hull.h"

#undef max
#undef min

// Convenience defines
#define DOT(x1,y1,x2,y2) ((x1)*(x2) + (y1)*(y2))
#define LENGTH(x,y) (sqrt((SQR(x))+(SQR(y))))
#define SIGN(x) (((x) >= 0.0) ? (1.0) : (-1.0) )

double calcHullDist(int startCellIdx, int nextCellIdx)
{
	return gHulls[startCellIdx].getCenterOfMass().EuclideanDistanceTo(gHulls[nextCellIdx].getCenterOfMass());
}

double calcCCDist(int startCellIdx, int nextCellIdx)
{
	if ( gHulls[startCellIdx].getFrame() > gHulls[nextCellIdx].getFrame() )
	{
		int tmp = startCellIdx;
		startCellIdx = nextCellIdx;
		nextCellIdx = tmp;
	}
	return gTrackerData.getDistance(startCellIdx,nextCellIdx,gCCMax);

	//if ( gCCDistMap[startCellIdx].count(nextCellIdx) == 0 )
	//	return gCCMax + 1.0;

	//return gCCDistMap[startCellIdx][nextCellIdx];
}

double calcFullCellDist(int startCellIdx, int nextCellIdx, double vmax, double ccmax)
{
	double hdist = calcHullDist(startCellIdx, nextCellIdx);

	if ( hdist > vmax )
		return dbltype::infinity();

	double cdist = calcCCDist(startCellIdx, nextCellIdx);

	if ( (cdist > ccmax) && (hdist > (vmax/2.0)) )
		return dbltype::infinity();

	int startCellSize = gHulls[startCellIdx].getNumberofVoxels();
	int nextCellSize = gHulls[nextCellIdx].getNumberofVoxels();

	int nmax = std::max<int>(startCellSize, nextCellSize);
	int nmin = std::min<int>(startCellSize, nextCellSize);

	double sdist = ((double) (nmax - nmin)) / nmax;

	return (10.0*hdist + 100.0*sdist + 1000.0*cdist);
}

double getCost(std::vector<int>& indices, int sourceIdx, int bCheck)
{
	double vmax = gVMax;
	double ccmax = gCCMax;

	if ( indices.size() - sourceIdx <= 1 )
		return dbltype::infinity();

	int sourceHull = indices[sourceIdx];
	int nextHull = indices[sourceIdx+1];

	int dir = 1;

	int startIdx;
	if ( bCheck )
		startIdx = indices.size() - 2;
	else
	{
		dir = (gHulls[nextHull].getFrame() - gHulls[sourceHull].getFrame() >= 0) ? 1 : -1;

		int tStart = std::max<int>(gHulls[indices[sourceIdx]].getFrame() - gWindowSize + 1, 0);
		int tPathStart = gHulls[indices[0]].getFrame();
		if ( dir < 0 )
		{
			tStart = std::max<int>(gHulls[indices[sourceIdx]].getFrame() + gWindowSize - 1, 0);
			tPathStart = gHulls[indices[0]].getFrame();
			startIdx = std::max<int>(tPathStart - tStart, 0);
		}
		else
			startIdx = std::max<int>(tStart - tPathStart, 0);
	}

	for ( int k=startIdx; k < indices.size()-1; ++k )
	{
		double dlcd = calcHullDist(indices[k], indices[k+1]);

		if ( dlcd > vmax )
			return dbltype::infinity();
	}

	// Just return non-infinite for a successful check.
	if ( bCheck )
		return 1.0;

	double localCost = 3*calcFullCellDist(indices[sourceIdx], indices[sourceIdx+1], vmax, ccmax);

	if ( localCost == dbltype::infinity() )
		return dbltype::infinity();

	// Calculate local surrounding connected-component distance if possible
	if ( sourceIdx > 0 )
		localCost += calcFullCellDist(indices[sourceIdx-1], indices[sourceIdx+1], 2*vmax, 2*ccmax);
	else
		localCost *= 2.0;

	if ( localCost == dbltype::infinity() )
		return dbltype::infinity();

	// Calculate forward cc cost if path is long enough
	if ( sourceIdx < indices.size()-2 )
		localCost += calcFullCellDist(indices[sourceIdx], indices[sourceIdx+2], 2*vmax, 2*ccmax);
	else
		localCost *= 2.0;

	if ( localCost == dbltype::infinity() )
		return dbltype::infinity();

	double dCenterLoc[3] = {0.0, 0.0, 0.0};

	// Calculate historical center of mass of cell
	for ( int k=startIdx; k <= sourceIdx; ++k )
	{
		dCenterLoc[0] += gHulls[indices[k]].getCenterOfMass().x;
		dCenterLoc[1] += gHulls[indices[k]].getCenterOfMass().y;
		dCenterLoc[2] += gHulls[indices[k]].getCenterOfMass().z;
	}
	dCenterLoc[0] /= (sourceIdx - startIdx + 1);
	dCenterLoc[1] /= (sourceIdx - startIdx + 1);
	dCenterLoc[2] /= (sourceIdx - startIdx + 1);

	// Calculate mean squared deviation of path from historical center
	double locationCost = 0.0;
	for ( int k=sourceIdx; k < indices.size(); ++k )
	{
		locationCost += SQR(gHulls[indices[k]].getCenterOfMass().x - dCenterLoc[0]) + SQR(gHulls[indices[k]].getCenterOfMass().y - dCenterLoc[1]) + SQR(gHulls[indices[k]].getCenterOfMass().z - dCenterLoc[2]);
	}
	locationCost = sqrt(locationCost/(indices.size() - sourceIdx));

	double totalCost = localCost + locationCost;
	if ( indices.size() < 2*gWindowSize+1 )
	{
		double lengthPenalty = (2*gWindowSize+1) - indices.size();
		totalCost *= (2*lengthPenalty);
	}

	return totalCost;
}