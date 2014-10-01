////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#include "tracker.h"
#include "Hull.h"

#include <set>

int buildHistoryPath(CSourcePath* historyPath, CSourcePath* path)
{
	int pathStartGIdx = path->indices[0];

	// Find the assigned in-edge
	int histTrackID = -1;
	int histIdx = gAssignedConnectIn[pathStartGIdx];
	if ( histIdx >=0 )
	{
		CSourcePath* histpath = gTrackerData.connectIn[pathStartGIdx][histIdx];
		histTrackID = histpath->trackletID;
	}

	if ( histTrackID >= 0 )
	{
		tPathList::iterator histIter = gAssignedTracklets[histTrackID].begin();
		while ( (histIter != gAssignedTracklets[histTrackID].end()) )
		{
			CSourcePath* curPath = *histIter;

			int startGIdx = curPath->indices[0];
			int endGIdx = gAssignedConnectOut[startGIdx];

			int endT = gHulls[endGIdx].getFrame();

			for (unsigned int i=0; gHulls[curPath->indices[i]].getFrame() < endT; ++i )
			{
				historyPath->pushPoint(curPath->indices[i]);
			}

			++histIter;
		}
	}

	for (unsigned int i=0; i < path->length(); ++i )
		historyPath->pushPoint(path->indices[i]);

	return histTrackID;
}

int addBestPath(CSourcePath &path, int bestGIdx)
{
	if ( path.length() > 1 )
	{
		CSourcePath historyPath;

		int startGIdx = path.indices[0];
		int nextGIdx = path.indices[1];

		int historyTrackID = buildHistoryPath(&historyPath, &path);
		int srcPathIdx = historyPath.length() - path.length();

		double newPathCost = getCost(historyPath.indices, srcPathIdx, 0);
		if ( newPathCost == dbltype::infinity() )
			return bestGIdx;

		path.trackletID = historyTrackID;
		path.cost = newPathCost;

		if ( gTrackerData.connectOut[startGIdx].count(nextGIdx) == 0 )
		{
			CSourcePath* newPath = new CSourcePath(path);
			gTrackerData.connectOut[startGIdx].insert(std::pair<int,CSourcePath*>(nextGIdx, newPath));
			gTrackerData.connectIn[nextGIdx].insert(std::pair<int,CSourcePath*>(startGIdx, newPath));
		}
		else if ( newPathCost < gTrackerData.connectOut[startGIdx][nextGIdx]->cost )
		{
			*(gTrackerData.connectOut[startGIdx][nextGIdx]) = path;
		}

		if ( (bestGIdx < 0) || (newPathCost < gTrackerData.connectOut[startGIdx][bestGIdx]->cost) )
		{
			bestGIdx = nextGIdx;
		}
	}

	return bestGIdx;
}

int bestDepthFirstPathSearch(CSourcePath partialPath, int bestGIdx, int t, int tEnd, int occlLookback)
{
	bool bFinishedSearch = true;
	if ( t < tEnd )
	{
		std::set<int>::iterator hullIter = gHashedHulls[t-occlLookback].begin();
		for ( ; hullIter != gHashedHulls[t-occlLookback].end(); ++hullIter )
		{
			int nextIdx = (*hullIter);

			partialPath.pushPoint(nextIdx);
			double chkCost = getCost(partialPath.indices, 0);
			partialPath.popPoint();

			if ( chkCost == dbltype::infinity() )
				continue;

			bFinishedSearch = false;

			partialPath.pushPoint(nextIdx);

			bestGIdx = bestDepthFirstPathSearch(partialPath, bestGIdx, t+1, tEnd, occlLookback);
			partialPath.popPoint();
		}
	}

	if ( bFinishedSearch )
	{
		bestGIdx = addBestPath(partialPath, bestGIdx);
	}

	return bestGIdx;
}

void BuildBestPaths(std::map<int,int>& bestOutEdges, int t, int occlLookback)
{
	if ( t-occlLookback < 0 )
		return;

	int tEnd = min((unsigned int)t+gWindowSize, (unsigned int)gHashedHulls.size());

	std::set<int>::iterator hullIter = gHashedHulls[t-occlLookback].begin();
	for ( ; hullIter != gHashedHulls[t-occlLookback].end(); ++hullIter )
	{
		int startGIdx = (*hullIter);
		if ( occlLookback > 0 && gAssignedConnectOut[startGIdx] >= 0 )
			continue;

		CSourcePath srcPath;
		srcPath.pushPoint(startGIdx);

		int bestGIdx = bestDepthFirstPathSearch(srcPath, -1, t+1, tEnd, occlLookback);

		if ( bestGIdx >= 0 )
			bestOutEdges[startGIdx] = bestGIdx;
	}
}