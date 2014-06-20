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