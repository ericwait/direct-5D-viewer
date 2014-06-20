#include "Utility.h"

Vec<float> gColors[NCOLORS] = {
	Vec<float>(1,0,0),
	Vec<float>(1,0.5,0),
	Vec<float>(1,1,0),
	Vec<float>(0,1,1),
	Vec<float>(0,0.5,1),
	Vec<float>(0.5,0,1),
	Vec<float>(1,0,0.5),
	Vec<float>(0,0.75,0.75),
	Vec<float>(0.75,0,0.75),
	Vec<float>(0.75,0.75,0),
	Vec<float>(0.7969,0,0.3984),
	Vec<float>(0.5977,0.3984,0),
	Vec<float>(0,0.7969,1),
	Vec<float>(1,0.5977,0.3984),
	Vec<float>(0.7969,0.5977,0)
};

Vec<float> PickColor()
{
	return gColors[rand()%NCOLORS];
}