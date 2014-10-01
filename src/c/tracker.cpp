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
#include "Track.h"

TrackerData gTrackerData;

int* gAssignedConnectOut;
int* gAssignedConnectIn;

std::vector<tPathList> gAssignedTracklets;

double gVMax = 20.0;//TODO try these values
double gCCMax = 10.0;

int gWindowSize = 4;

TrackerData::TrackerData()
{
	connectIn = NULL;
	connectOut = NULL;
	ccDistMap = NULL;
}

TrackerData::~TrackerData()
{
	clear();
}

void TrackerData::clear()
{
	if ( connectOut )
	{
		delete[] connectOut;
		connectOut = NULL;
	}

	if ( connectIn )
	{
		delete[] connectIn;
		connectIn = NULL;
	}

	if (ccDistMap)
	{
		delete[] ccDistMap;
		ccDistMap = NULL;
	}
}

int FindMinInEdgeIdx(int nextGIdx)
{
	double cmin = dbltype::infinity();
	int bestIdx = -1;

	std::map<int,CSourcePath*>::iterator cIter = gTrackerData.connectIn[nextGIdx].begin();
	while ( cIter != gTrackerData.connectIn[nextGIdx].end() )
	{
		CSourcePath* inPath = cIter->second;
		if ( inPath->cost < cmin )
		{
			cmin = inPath->cost;
			bestIdx = cIter->first;
		}

		++cIter;
	}

	return bestIdx;
}

void resetIdxTrackletIds(int nodeIdx, int newTrackletID)
{
	std::map<int,CSourcePath*>::iterator pathIter = gTrackerData.connectOut[nodeIdx].begin();
	while ( pathIter != gTrackerData.connectOut[nodeIdx].end() )
	{
		pathIter->second->trackletID = newTrackletID;

		++pathIter;
	}
}

void resetForwardTrackletIds(int startingGIdx, int newTrackletID)
{
	int oldTrackletID = -1;
	int nxtGIdx = gAssignedConnectOut[startingGIdx];
	if ( nxtGIdx >= 0 )
		oldTrackletID = gTrackerData.connectOut[startingGIdx][nxtGIdx]->trackletID;

	resetIdxTrackletIds(startingGIdx, newTrackletID);
	while ( nxtGIdx >= 0 )
	{
		CSourcePath* curPath = gTrackerData.connectOut[startingGIdx][nxtGIdx];
		gAssignedTracklets[newTrackletID].push_back(curPath);

		resetIdxTrackletIds(nxtGIdx, newTrackletID);

		startingGIdx = nxtGIdx;
		nxtGIdx = gAssignedConnectOut[startingGIdx];
	}

	if ( oldTrackletID >= 0 )
		gAssignedTracklets[oldTrackletID].clear();
}

void patchUpResults()
{
	std::multimap<double,std::pair<int,int>> patchupList;
	// Make sorted list of possible patch assignments (maps automatically sort internally)
	for ( int i=0; i < gHulls.size(); ++i )
	{
		if ( gAssignedConnectIn[i] >= 0 )
			continue;

		std::map<int,CSourcePath*>::iterator inIter = gTrackerData.connectIn[i].begin();
		while ( inIter != gTrackerData.connectIn[i].end() )
		{
			int inGIdx = inIter->first;
			if ( gAssignedConnectOut[inGIdx] < 0 )
			{
				patchupList.insert(std::pair<double,std::pair<int,int>>(inIter->second->cost, std::pair<int,int>(inGIdx,i)));
			}

			++inIter;
		}
	}

	int totalPatches = 0;
	std::multimap<double,std::pair<int,int>>::iterator srtIter = patchupList.begin();
	while ( srtIter != patchupList.end() )
	{
		int startGIdx = srtIter->second.first;
		int destGIdx = srtIter->second.second;

		CSourcePath* curPath = gTrackerData.connectOut[startGIdx][destGIdx];
		if ( gAssignedConnectOut[startGIdx] < 0 && gAssignedConnectIn[destGIdx] < 0 )
		{
			// Get Tracklet ID first from incoming assignment
			int newTrackletID = curPath->trackletID;
			if ( newTrackletID < 0 )
			{
				// Or from incoming assignment made during patching
				int prevIdx = gAssignedConnectIn[startGIdx];
				if ( prevIdx >= 0 )
				{
					newTrackletID = gTrackerData.connectOut[prevIdx][startGIdx]->trackletID;
					curPath->trackletID = newTrackletID;
				}
			}
			//if ( newTrackletID < 0 )
			//{
			//	// Or from outgoing assignment if the other two don't exist
			//	int nxtGIdx = gAssignedConnectOut[destGIdx];
			//	if ( nxtGIdx >= 0 )
			//	{
			//		newTrackletID = gTrackerData.connectOut[destGIdx][nxtGIdx]->trackletID;
			//		curPath->trackletID = newTrackletID;
			//	}
			//}
			if ( newTrackletID < 0 )
			{
				//Otherwise add new tracklet to list etc. and set id
				newTrackletID = gAssignedTracklets.size();
				curPath->trackletID = newTrackletID;

				tPathList newList;
				gAssignedTracklets.push_back(newList);
			}

			int mergeTrackletId = -1;
			int nxtGIdx = gAssignedConnectOut[destGIdx];
			if ( nxtGIdx >= 0 )
			{
				mergeTrackletId = gTrackerData.connectOut[destGIdx][nxtGIdx]->trackletID;
			}

			//Add path to tracklet list
			gAssignedTracklets[newTrackletID].push_back(curPath);

			if ( mergeTrackletId >= 0 )
				resetForwardTrackletIds(destGIdx, newTrackletID);

			//Keep track of assignment for fast lookup
			gAssignedConnectIn[destGIdx] = startGIdx;
			gAssignedConnectOut[startGIdx] = destGIdx;

			++totalPatches;
		}

		++srtIter;
	}
}

void destroyTrackStructures()
{
	if ( gTrackerData.connectOut )
	{
		delete[] gTrackerData.connectOut;
		gTrackerData.connectOut = NULL;
	}

	if ( gTrackerData.connectIn )
	{
		delete[] gTrackerData.connectIn;
		gTrackerData.connectIn = NULL;
	}

	if ( gAssignedConnectOut )
	{
		delete[] gAssignedConnectOut;
		gAssignedConnectOut = NULL;
	}

	if ( gAssignedConnectIn )
	{
		delete[] gAssignedConnectIn;
		gAssignedConnectIn = NULL;
	}

	gTracks.clear();
	gAssignedTracklets.clear();
}

void initTrackStrucutres()
{
	destroyTrackStructures();

	gTrackerData.connectOut = new std::map<int,CSourcePath*>[gHulls.size()];
	gTrackerData.connectIn = new std::map<int,CSourcePath*>[gHulls.size()];

	gAssignedConnectOut = new int[gHulls.size()];
	gAssignedConnectIn = new int[gHulls.size()];

	for ( int i=0; i < gHulls.size(); ++i )
	{
		gAssignedConnectOut[i] = -1;
		gAssignedConnectIn[i] = -1;
	}
}

//void setHullInfo(int hullID, int trackID)
//{
//	//gHulls[hullID].label = trackID;
//	for ( int i=0; i < gHulls[hullID].getNumberOfVertices(); ++i )
//	{
//		gHulls[hullID].getMesh()->m_vertices[i].texUV = gTracks[trackID]->getColor();
//	}
//}

void buildTracks()
{
	gTracks.clear();
	for ( int i=0; i < gAssignedTracklets.size(); ++i )
	{
		if ( gAssignedTracklets[i].empty() )
			continue;

		Track curTrack((unsigned int)gTracks.size());

		tPathList::iterator pathIter = gAssignedTracklets[i].begin();
		int startIdx = (*pathIter)->indices[0];
		int endIdx = gAssignedTracklets[i].back()->indices[1];

		gHulls[startIdx].setTrack((unsigned int)gTracks.size());

		curTrack.setColor(PickColor());
		gHulls[startIdx].setColor(curTrack.getColor());

		curTrack.addHull(gHulls[startIdx].getFrame(),startIdx);
		for ( ; pathIter != gAssignedTracklets[i].end(); ++pathIter )
		{
			int startIdx = (*pathIter)->indices[0];
			int nextIdx = (*pathIter)->indices[1];

			//setHullInfo(nextIdx, trackID);
			gHulls[nextIdx].setTrack((unsigned int)gTracks.size());
			gHulls[nextIdx].setColor(curTrack.getColor());

			int tDiff = gHulls[nextIdx].getFrame()- gHulls[startIdx].getFrame();
			//for ( int k=0; k < tDiff; ++k )
			//	curTrack->hulls.push_back(-1);

			curTrack.addHull(gHulls[nextIdx].getFrame(),nextIdx);
		}

		gTracks.push_back(curTrack);

		//Family fam(gFamilies.size());
		//fam.addSubTree(curTrack.getLabel());
		//gFamilies.push_back(fam);
	}
}


void trackHulls(int numFrames)
{
	//if (!gTracks.empty())
	//	return;

	//if (0==frame)
	//{
	buildConnectedComponentDist(2);
	initTrackStrucutres();
	//}

	std::map<int,int> bestOutEdges;

	for (unsigned int frame=0; frame<numFrames; ++frame)
	{

		if (gHashedHulls.size()>frame+1)
			//for ( int frame=0; frame < gHashedHulls.size()-1; ++frame )
		{

#ifdef _DEBUG
			char buffer[255];
			sprintf_s(buffer,"Tracking frame: %d",frame);
			printf(buffer);
#endif // _DEBUG

			
			bestOutEdges.clear();
			BuildBestPaths(bestOutEdges, frame);

			//Occlusions
			for ( int iLookback=1; iLookback <= 1; ++iLookback )
			{
				//TODO Fix comparison occlusion handling 
				BuildBestPaths(bestOutEdges, frame, iLookback);
			}

			std::set<int>::iterator nextIdxIter = gHashedHulls[frame+1].begin();
			for ( ; nextIdxIter != gHashedHulls[frame+1].end(); ++nextIdxIter)
			{
				int nextGIdx = *nextIdxIter;
				int bestTrackletIdx = FindMinInEdgeIdx(nextGIdx);
				if ( bestTrackletIdx < 0 )
					continue;

				if ( (bestOutEdges.count(bestTrackletIdx) == 0) || bestOutEdges[bestTrackletIdx] != nextGIdx )
					continue;

				int newTrackletID = gTrackerData.connectOut[bestTrackletIdx][nextGIdx]->trackletID;

				if ( newTrackletID < 0 )
				{
					//Add new tracklet to list etc. and set id
					newTrackletID = gAssignedTracklets.size();
					gTrackerData.connectOut[bestTrackletIdx][nextGIdx]->trackletID = newTrackletID;

					tPathList newList;
					gAssignedTracklets.push_back(newList);
				}

				//Add path to tracklet list
				gAssignedTracklets[newTrackletID].push_back(gTrackerData.connectOut[bestTrackletIdx][nextGIdx]);

				//Keep track of assignment for fast lookup
				gAssignedConnectIn[nextGIdx] = bestTrackletIdx;
				gAssignedConnectOut[bestTrackletIdx] = nextGIdx;
			}
		}
	}

	patchUpResults();

	buildTracks();

	for (unsigned int i=0; i<gHulls.size(); ++i)
	{
		if (UNSET_VAR==gHulls[i].getTrack())
		{
			Track curTrack((unsigned int)gTracks.size());
			gHulls[i].setTrack((unsigned int)gTracks.size());

			curTrack.setColor(PickColor());
			gHulls[i].setColor(curTrack.getColor());

			curTrack.addHull(gHulls[i].getFrame(),i);

			gTracks.push_back(curTrack);
		}
	}
}

void TrackerData::setCCdistMap(std::map<int,double>* cCDistMap)
{
	if (NULL!=cCDistMap)
	{
		if (NULL!=this->ccDistMap)
		{
			delete[] this->ccDistMap;
			ccDistMap = NULL;
		}

		this->ccDistMap = cCDistMap;
	}
}

double TrackerData::getDistance(int hull1, int hull2, double ccMax)
{
	if ( gTrackerData.ccDistMap[hull1].count(hull2) == 0 )
		return ccMax + 1.0;

	return ccDistMap[hull1][hull2];
}
