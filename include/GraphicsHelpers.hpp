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
	v1.vec.x() *= (width  - 1);
	v1.vec.y() *= (height - 1);
	v2.vec.x() *= (width  - 1);
	v2.vec.y() *= (height - 1);
	v3.vec.x() *= (width  - 1);
	v3.vec.y() *= (height - 1);

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

	int y1Sorted = std::ceil( v1.vec.y() );
	int y2Sorted = std::ceil( v2.vec.y() );
	int y3Sorted = std::ceil( v3.vec.y() );

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

inline void triSortVertices (int& x1Sorted, int& y1Sorted, float& x1FSorted, float& y1FSorted,
				int& x2Sorted, int& y2Sorted, float& x2FSorted, float& y2FSorted,
				int& x3Sorted, int& y3Sorted, float& x3FSorted, float& y3FSorted)
{
	// first sort by y values
	if (y2Sorted > y3Sorted)
	{
		int xSTemp = x2Sorted;
		int ySTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xSTemp;
		y3Sorted = ySTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
	}
	if (y1Sorted > y2Sorted)
	{
		int xSTemp = x1Sorted;
		int ySTemp = y1Sorted;
		float xFTemp = x1FSorted;
		float yFTemp = y1FSorted;
		x1Sorted = x2Sorted;
		y1Sorted = y2Sorted;
		x2Sorted = xSTemp;
		y2Sorted = ySTemp;
		x1FSorted = x2FSorted;
		y1FSorted = y2FSorted;
		x2FSorted = xFTemp;
		y2FSorted = yFTemp;
	}
	if (y2Sorted > y3Sorted)
	{
		int xSTemp = x2Sorted;
		int ySTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xSTemp;
		y3Sorted = ySTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
	}

	// then sort by x values
	if (y2Sorted == y3Sorted && x2Sorted > x3Sorted)
	{
		int xSTemp = x2Sorted;
		int ySTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xSTemp;
		y3Sorted = ySTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
	}
	if (y1Sorted == y2Sorted && x1Sorted > x2Sorted)
	{
		int xSTemp = x1Sorted;
		int ySTemp = y1Sorted;
		float xFTemp = x1FSorted;
		float yFTemp = y1FSorted;
		x1Sorted = x2Sorted;
		y1Sorted = y2Sorted;
		x2Sorted = xSTemp;
		y2Sorted = ySTemp;
		x1FSorted = x2FSorted;
		y1FSorted = y2FSorted;
		x2FSorted = xFTemp;
		y2FSorted = yFTemp;
	}
	if (y2Sorted == y3Sorted && x2Sorted > x3Sorted)
	{
		int xSTemp = x2Sorted;
		int ySTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xSTemp;
		y3Sorted = ySTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
	}
}

#endif // GRAPHICSHELPERS_HPP
