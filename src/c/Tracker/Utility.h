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

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <Windows.h>

#define SQR(x) ((x)*(x))

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#define MatToC(x) (x-1)
#define CToMat(x) (x+1)

#define UNSET_VAR (-1)
#define NCOLORS (15)

#define MAX(x,y) ((x>y)?(x):(y))
#define MIN(x,y) ((x<y)?(x):(y))

template<typename T>
struct Vec
{
	T x;
	T y;
	T z;

	Vec()
	{
		x=0;
		y=0;
		z=0;
	}

	Vec(T x, T y, T z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec<T> operator+ (Vec<T> other) const
	{
		Vec<T> outVec;
		outVec.x = x + other.x;
		outVec.y = y + other.y;
		outVec.z = z + other.z;

		return outVec;
	}

	Vec<T> operator- (Vec<T> other) const
	{
		Vec<T> outVec;
		outVec.x = x - other.x;
		outVec.y = y - other.y;
		outVec.z = z - other.z;

		return outVec;
	}

	Vec<T> operator- () const
	{
		Vec<T> outVec;
		outVec.x = -x;
		outVec.y = -y;
		outVec.z = -z;

		return outVec;
	}

	Vec<T>& operator= (const Vec<T> inVec)
	{
		x = inVec.x;
		y = inVec.y;
		z = inVec.z;

		return *this;
	}

	size_t product() const
	{
		return x*y*z;
	}

	double EuclideanDistanceTo(Vec<T> other)
	{
		return sqrt(SQR((double)x-other.x)+SQR((double)y-other.y)+SQR((double)z-other.z));
	}

	double SquareDistanceTo(Vec<T> other)
	{
		return SQR((double)x-other.x)+SQR((double)y-other.y)+SQR((double)z-other.z);
	}
};

extern Vec<float> gColors[NCOLORS];

Vec<float> PickColor();

#endif