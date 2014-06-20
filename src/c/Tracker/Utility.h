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