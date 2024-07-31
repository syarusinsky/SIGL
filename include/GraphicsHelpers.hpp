#ifndef GRAPHICSHELPERS_HPP
#define GRAPHICSHELPERS_HPP

#include "Engine3D.hpp"

#include <algorithm>
#include <limits>
#include <math.h>

inline bool floatsAreEqual (float x, float y, float diff = std::numeric_limits<float>::epsilon() )
{
	if ( fabs(x - y) < diff ) return true;

	return false;
}

inline void triSortVertices (Vertex& v1, Vertex& v2, Vertex& v3, float width, float height)
{
	// first sort by y values
	if ( v2.vec.y() > v3.vec.y() )
	{
		Vertex temp = v2;
		v2 = v3;
		v3 = temp;
	}
	if ( v1.vec.y() > v2.vec.y() )
	{
		Vertex temp = v1;
		v1 = v2;
		v2 = temp;
	}
	if ( v2.vec.y() > v3.vec.y() )
	{
		Vertex temp = v2;
		v2 = v3;
		v3 = temp;
	}

	float y1Sorted = v1.vec.y();
	float y2Sorted = v2.vec.y();
	float y3Sorted = v3.vec.y();

	// then sort by x values
	if ( y2Sorted == y3Sorted && v2.vec.x() > v3.vec.x() )
	{
		Vertex temp = v2;
		v2 = v3;
		v3 = temp;
	}
	if ( y1Sorted == y2Sorted && v1.vec.x() > v2.vec.x() )
	{
		Vertex temp = v1;
		v1 = v2;
		v2 = temp;
	}
	if ( y2Sorted == y3Sorted && v2.vec.x() > v3.vec.x() )
	{
		Vertex temp = v2;
		v2 = v3;
		v3 = temp;
	}
}

inline void triSortVertices (float& x1Sorted, float& y1Sorted,
				float& x2Sorted, float& y2Sorted,
				float& x3Sorted, float& y3Sorted)
{
	// first sort by y values
	if (y2Sorted > y3Sorted)
	{
		float xSTemp = x2Sorted;
		float ySTemp = y2Sorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xSTemp;
		y3Sorted = ySTemp;
	}
	if (y1Sorted > y2Sorted)
	{
		float xSTemp = x1Sorted;
		float ySTemp = y1Sorted;
		x1Sorted = x2Sorted;
		y1Sorted = y2Sorted;
		x2Sorted = xSTemp;
		y2Sorted = ySTemp;
	}
	if (y2Sorted > y3Sorted)
	{
		float xSTemp = x2Sorted;
		float ySTemp = y2Sorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xSTemp;
		y3Sorted = ySTemp;
	}

	// then sort by x values
	if (y2Sorted == y3Sorted && x2Sorted > x3Sorted)
	{
		float xSTemp = x2Sorted;
		float ySTemp = y2Sorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xSTemp;
		y3Sorted = ySTemp;
	}
	if (y1Sorted == y2Sorted && x1Sorted > x2Sorted)
	{
		float xSTemp = x1Sorted;
		float ySTemp = y1Sorted;
		x1Sorted = x2Sorted;
		y1Sorted = y2Sorted;
		x2Sorted = xSTemp;
		y2Sorted = ySTemp;
	}
	if (y2Sorted == y3Sorted && x2Sorted > x3Sorted)
	{
		float xSTemp = x2Sorted;
		float ySTemp = y2Sorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xSTemp;
		y3Sorted = ySTemp;
	}
}

#endif // GRAPHICSHELPERS_HPP
