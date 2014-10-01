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

#include "Hull.h"

#include <set>

std::vector<Hull> gHulls;
std::vector<std::set<int>> gHashedHulls;

void Hull::clear()
{
	frame = UNSET_VAR;
	interiorVoxelIndices.clear();
	centerOfMass.x = UNSET_VAR;
	centerOfMass.y = UNSET_VAR;
	centerOfMass.z = UNSET_VAR;
	track = UNSET_VAR;
	clearBoundingBox();
}

void Hull::clearBoundingBox()
{
	boundingBox[0] = UNSET_VAR;
	boundingBox[1] = UNSET_VAR;
	boundingBox[2] = UNSET_VAR;
	boundingBox[3] = 0;
	boundingBox[4] = 0;
	boundingBox[5] = 0;
}

Hull::Hull()
{
	clear();
}

Hull::Hull(unsigned int frame)
{
	clear();
	this->frame = frame;
}

Hull::~Hull()
{
	clear();
}

void Hull::getColor(double* colorOut)
{
	colorOut[0] = (double)color.x;
	colorOut[1] = (double)color.y;
	colorOut[2] = (double)color.z;
}

void Hull::setCenterOfMass(double* com)
{
	centerOfMass.x = com[0];
	centerOfMass.y = com[1];
	centerOfMass.z = com[2];
}

PointType Hull::getNthVoxel(unsigned int n)
{
	PointType returnVoxel;
	if (n>=interiorVoxelIndices.size())
	{
		returnVoxel.x = 0;
		returnVoxel.y = 0;
		returnVoxel.z = 0;
	}else
		returnVoxel = interiorVoxelIndices.at(n);

	return returnVoxel;
}

void Hull::setBoundingBox(double* bb)
{
	boundingBox[0] = bb[3];
	boundingBox[1] = bb[4];
	boundingBox[2] = bb[2];
	boundingBox[3] = bb[0];
	boundingBox[4] = bb[1];
	boundingBox[5] = bb[5];
}

void Hull::setFrame(unsigned int frame)
{
	this->frame = frame;
}