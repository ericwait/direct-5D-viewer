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

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include <list>
#include <vector>

#include "cost.h"
#include "paths.h"
#include "helperutils.h"

//For quick edge lookup from point (like inID/outID)
extern int* gAssignedConnectOut;
extern int* gAssignedConnectIn;

//Global variables
extern int gWindowSize;

extern double gVMax;
extern double gCCMax;

typedef std::list<CSourcePath*> tPathList;
extern std::vector<tPathList> gAssignedTracklets;

//main tracker function:
void trackHulls(int numFrames);

void destroyTrackStructures();

class TrackerData
{
public:
	TrackerData();
	~TrackerData();

	//savers
	void clear();

	double getDistance(int hull1, int hull2, double ccMax);
	void setCCdistMap(std::map<int,double>* cCDistMap);

	//member variables
	std::map<int,CSourcePath*>* connectOut;
	std::map<int,CSourcePath*>* connectIn;
	std::map<int,double>* ccDistMap;
};

extern TrackerData gTrackerData;

#endif //_TRACKER_H_