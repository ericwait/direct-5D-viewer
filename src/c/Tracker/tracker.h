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