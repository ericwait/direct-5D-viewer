#pragma once

#include "Vec.h"

struct Color
{
	union
	{
		float e[4];
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
	};

	Color(): r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}

	Color(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{}

	Color(Vec<float> rgb, float alpha)
		: r(rgb.e[0]), g(rgb.e[1]), b(rgb.e[2]), a(alpha)
	{}
};
