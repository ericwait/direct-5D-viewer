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