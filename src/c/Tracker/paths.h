#ifndef PATHS_H
#define PATHS_H

#include "Utility.h"

#include <climits>
#include <vector>
#include <map>

typedef std::numeric_limits<double> dbltype;

class CSourcePath
{
public:
	CSourcePath()
	{
		trackletID = UNSET_VAR;
		cost = dbltype::infinity();
	}

	unsigned int length()
	{
		return (unsigned int)indices.size();
	}

	void pushPoint(int newIdx)
	{
		indices.push_back(newIdx);
	}

	void popPoint()
	{
		if ( indices.size() <= 1 )
			return;

		indices.pop_back();
	}

	void clear()
	{
		trackletID = UNSET_VAR;
		cost = dbltype::infinity();

		indices.clear();
	}

public:

	int trackletID;
	double cost;

	std::vector<int> indices;
};

void BuildBestPaths(std::map<int,int>& bestOutEdges, int t, int occlLookcback = 0);

#endif