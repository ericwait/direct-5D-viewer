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