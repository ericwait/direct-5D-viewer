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

#ifndef HULL_H
#define HULL_H

#include "Utility.h"
#include <vector>
#include <set>

typedef Vec<double> PointType;

class Hull
{
public:
	Hull();
	Hull(unsigned int frame);
	~Hull();

	//getters
	unsigned int getFrame(){return frame;}
	Vec<double> getCenterOfMass(){return centerOfMass;}
	std::vector<PointType> getInteriorVoxelIndices(){return interiorVoxelIndices;}
	size_t getNumberofVoxels(){return interiorVoxelIndices.size();}
	PointType getNthVoxel(unsigned int n);
	unsigned int getTrack(){return track;}
	double getBoundingBoxXmin(){return boundingBox[0];}
	double getBoundingBoxYmin(){return boundingBox[1];}
	double getBoundingBoxZmin(){return boundingBox[2];}
	double getBoundingBoxXmax(){return boundingBox[3];}
	double getBoundingBoxYmax(){return boundingBox[4];}
	double getBoundingBoxZmax(){return boundingBox[5];}
	double* const getBoundingBox(){return boundingBox;}
	Vec<float> getColor(){return color;}
	void getColor(double* colorOut);

	//setters
	void setCenterOfMass(double* com);
	void setFrame(unsigned int frame);
	void setTrack(double* label){this->track=*label;}
	void setTrack(int label){this->track=label;}
	void setBoundingBox(double* boundingBox);
	void setColor(Vec<float> color){this->color = color;}
	void setInteriorVoxelIndices(std::vector<PointType> voxelIndices){interiorVoxelIndices=voxelIndices;}

	void clearTrack(){track=UNSET_VAR;}
	void logicallyDelete(unsigned int hull);

private:
	unsigned int frame;
	Vec<double> centerOfMass;
	unsigned int track;
	std::vector<PointType> interiorVoxelIndices;
	double boundingBox[6];// {xMin,yMin,zMin,xMax,yMax,zMax}
	Vec<float> color;

	void clear();
	void clearBoundingBox();
};

extern std::vector<Hull> gHulls;
extern std::vector<std::set<int>> gHashedHulls;

#endif