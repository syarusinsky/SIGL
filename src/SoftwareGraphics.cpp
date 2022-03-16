#include "SoftwareGraphics.hpp"

#include "Font.hpp"
#include "Sprite.hpp"

#include <algorithm>
#include <limits>

SoftwareGraphics::SoftwareGraphics (FrameBuffer* frameBuffer) :
	Graphics( frameBuffer )
{
}

SoftwareGraphics::~SoftwareGraphics()
{
}

void SoftwareGraphics::setColor (float r, float g, float b)
{
	m_ColorProfile->setColor( r, g, b );
}

void SoftwareGraphics::setColor (bool val)
{
	m_ColorProfile->setColor( val );
}

void SoftwareGraphics::setFont (Font* font)
{
	m_CurrentFont = font;
}

void SoftwareGraphics::fill()
{
	for (unsigned int pixel = 0; pixel < (m_FBWidth * m_FBHeight); pixel++)
	{
		m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );
	}
}

void SoftwareGraphics::drawLine (float xStart, float yStart, float xEnd, float yEnd)
{
	// clip line and return if line is off screen
	if ( !clipLine( &xStart, &yStart, &xEnd, &yEnd ) ) return;

	unsigned int xStartUInt = xStart * (m_FBWidth  - 1);
	unsigned int yStartUInt = yStart * (m_FBHeight - 1);
	unsigned int xEndUInt   = xEnd   * (m_FBWidth  - 1);
	unsigned int yEndUInt   = yEnd   * (m_FBHeight - 1);

	float slope = ((float) yEndUInt - yStartUInt) / ((float)xEndUInt - xStartUInt);

	unsigned int pixelStart = ( (m_FBWidth * yStartUInt) + xStartUInt );
	unsigned int pixelEnd   = ( (m_FBWidth * yEndUInt  ) + xEndUInt   );

	if (pixelStart > pixelEnd)
	{
		// swap
		unsigned int temp = pixelStart;
		pixelStart = pixelEnd;
		pixelEnd   = temp;
	}

	unsigned int pixel = pixelStart;
	float xAccumulator = slope;
	float yAccumulator = slope;

	if (slope >= 0.0f)
	{
		while (pixel <= pixelEnd)
		{
			// x stride
			if (std::isinf(slope)) // don't stride along the x-axis at all
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );
			}
			else if (slope < 1.0f) // stride along x-axis across multiple pixels
			{
				while (xAccumulator < 1.0f && pixel <= pixelEnd)
				{
					m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

					xAccumulator += slope;
					pixel += 1;
				}
				xAccumulator = xAccumulator - 1.0f;
			}
			else // stride along x-axis by one pixel
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

				pixel += 1;
			}

			// y stride
			if (slope >= 1.0f) // stride along the y-axis across multiple pixels
			{
				while (yAccumulator >= 1.0f && pixel <= pixelEnd)
				{
					m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

					yAccumulator -= 1.0f;
					pixel += m_FBWidth;
				}
				yAccumulator += slope;
			}
			else // stride along the y-axis by one pixel
			{
				pixel += m_FBWidth;
			}
		}
	}
	else // slope is less than 0.0f
	{
		while (pixel < pixelEnd)
		{
			// x stride
			if (std::isinf(slope)) // don't stride along the x-axis at all
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );
			}
			else if (slope > -1.0f) // stride along x-axis across multiple pixels
			{
				while (xAccumulator > -1.0f && pixel <= pixelEnd)
				{
					m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

					xAccumulator += slope;
					pixel -= 1;
				}
				xAccumulator = xAccumulator + 1.0f;
			}
			else // stride along x-axis by one pixel
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

				pixel -= 1;
			}

			// y stride
			if (slope <= -1.0f) // stride along the y-axis across multiple pixels
			{
				while (yAccumulator <= 1.0f && pixel <= pixelEnd)
				{
					m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

					yAccumulator += 1.0f;
					pixel += m_FBWidth;
				}
				yAccumulator += slope;
			}
			else // stride along the y-axis by one pixel
			{
				pixel += m_FBWidth;

				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );
			}
		}
	}
}

void SoftwareGraphics::drawBox (float xStart, float yStart, float xEnd, float yEnd)
{
	drawLine( xStart, yStart, xEnd,   yStart );
	drawLine( xEnd,   yStart, xEnd,   yEnd   );
	drawLine( xEnd,   yEnd,   xStart, yEnd   );
	drawLine( xStart, yEnd,   xStart, yStart );
}

void SoftwareGraphics::drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd)
{
	int xStartUInt = xStart * (m_FBWidth  - 1);
	int yStartUInt = yStart * (m_FBHeight - 1);
	int xEndUInt   = xEnd   * (m_FBWidth  - 1);
	int yEndUInt   = yEnd   * (m_FBHeight - 1);

	// clipping
	xStartUInt = std::min( std::max(xStartUInt, 0), (int)m_FBWidth );
	yStartUInt = std::min( std::max(yStartUInt, 0), (int)m_FBHeight );
	xEndUInt   = std::min( std::max(xEndUInt,   0), (int)m_FBWidth );
	yEndUInt   = std::min( std::max(yEndUInt,   0), (int)m_FBHeight );

	unsigned int pixelRowStride = abs( (int)xEndUInt - (int)xStartUInt );

	unsigned int pixelStart = ( (m_FBWidth * yStartUInt) + xStartUInt );
	unsigned int pixelEnd   = ( (m_FBWidth * yEndUInt  ) + xEndUInt   );

	if (pixelStart > pixelEnd)
	{
		// swap
		unsigned int temp = pixelStart;
		pixelStart = pixelEnd;
		pixelEnd   = temp;
	}

	for (unsigned int pixel = pixelStart; pixel < pixelEnd; pixel += m_FBWidth)
	{
		unsigned int pixelRowEnd = pixel + pixelRowStride;

		for (unsigned int rowPixel = pixel; rowPixel < pixelRowEnd; rowPixel += 1)
		{
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, rowPixel );
		}
	}
}

void SoftwareGraphics::drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x1, y1 );
}

static inline void triSortVertices (int& x1Sorted, int& y1Sorted, float& x1FSorted, float& y1FSorted,
									int& x2Sorted, int& y2Sorted, float& x2FSorted, float& y2FSorted,
									int& x3Sorted, int& y3Sorted, float& x3FSorted, float& y3FSorted )
{
	// first sort by y values
	if (y2Sorted > y3Sorted)
	{
		int xTemp = x2Sorted;
		int yTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xTemp;
		y3Sorted = yTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
	}
	if (y1Sorted > y2Sorted)
	{
		int xTemp = x1Sorted;
		int yTemp = y1Sorted;
		float xFTemp = x1FSorted;
		float yFTemp = y1FSorted;
		x1Sorted = x2Sorted;
		y1Sorted = y2Sorted;
		x2Sorted = xTemp;
		y2Sorted = yTemp;
		x1FSorted = x2FSorted;
		y1FSorted = y2FSorted;
		x2FSorted = xFTemp;
		y2FSorted = yFTemp;
	}
	if (y2Sorted > y3Sorted)
	{
		int xTemp = x2Sorted;
		int yTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xTemp;
		y3Sorted = yTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
	}

	// then sort by x values
	if (y2Sorted == y3Sorted && x2Sorted > x3Sorted)
	{
		int xTemp = x2Sorted;
		int yTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xTemp;
		y3Sorted = yTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
	}
	if (y1Sorted == y2Sorted && x1Sorted > x2Sorted)
	{
		int xTemp = x1Sorted;
		int yTemp = y1Sorted;
		float xFTemp = x1FSorted;
		float yFTemp = y1FSorted;
		x1Sorted = x2Sorted;
		y1Sorted = y2Sorted;
		x2Sorted = xTemp;
		y2Sorted = yTemp;
		x1FSorted = x2FSorted;
		y1FSorted = y2FSorted;
		x2FSorted = xFTemp;
		y2FSorted = yFTemp;
	}
	if (y2Sorted == y3Sorted && x2Sorted > x3Sorted)
	{
		int xTemp = x2Sorted;
		int yTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xTemp;
		y3Sorted = yTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
	}
}

void SoftwareGraphics::drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3)
{
	// getting the pixel values of the vertices
	int x1UInt = x1 * (m_FBWidth  - 1);
	int y1UInt = y1 * (m_FBHeight - 1);
	int x2UInt = x2 * (m_FBWidth  - 1);
	int y2UInt = y2 * (m_FBHeight - 1);
	int x3UInt = x3 * (m_FBWidth  - 1);
	int y3UInt = y3 * (m_FBHeight - 1);

	int x1Sorted = x1UInt;
	int y1Sorted = y1UInt;
	int x2Sorted = x2UInt;
	int y2Sorted = y2UInt;
	int x3Sorted = x3UInt;
	int y3Sorted = y3UInt;
	float x1FSorted = x1;
	float y1FSorted = y1;
	float x2FSorted = x2;
	float y2FSorted = y2;
	float x3FSorted = x3;
	float y3FSorted = y3;

	// sorting vertices
	triSortVertices( x1Sorted, y1Sorted, x1FSorted, y1FSorted, x2Sorted, y2Sorted, x2FSorted, y2FSorted,
						x3Sorted, y3Sorted, x3FSorted, y3FSorted );

	// getting the slope of each line
	float line1Slope = ((float) y2Sorted - y1Sorted) / ((float) x2Sorted - x1Sorted);
	float line2Slope = ((float) y3Sorted - y1Sorted) / ((float) x3Sorted - x1Sorted);
	float line3Slope = ((float) y3Sorted - y2Sorted) / ((float) x3Sorted - x2Sorted);

	// floats for x-intercepts (assuming the top of the triangle is pointed for now)
	float xLeftAccumulator  = (float) x1Sorted;
	float xRightAccumulator = (float) x1Sorted;

	// floats for incrementing xLeftAccumulator and xRightAccumulator
	float xLeftIncrTop     = 1.0f / line1Slope;
	float xRightIncrTop    = 1.0f / line2Slope;
	float xLeftIncrBottom  = 1.0f / line3Slope;
	float xRightIncrBottom = 1.0f / line2Slope;

	// xLeftIncrBottom < xRightIncrBottom is a substitute for line2Slope being on the top or bottom
	bool needsSwapping = (xLeftIncrBottom < xRightIncrBottom);

	// depending on the position of the vertices, we need to swap increments
	if ( (needsSwapping && x1Sorted < x2Sorted) || (needsSwapping && x1Sorted >= x2Sorted && x2Sorted > x3Sorted) )
	{
		float tempIncr = xLeftIncrTop;
		xLeftIncrTop = xRightIncrTop;
		xRightIncrTop = tempIncr;

		tempIncr = xLeftIncrBottom;
		xLeftIncrBottom = xRightIncrBottom;
		xRightIncrBottom = tempIncr;
	}

	// if slope is zero, the top of the triangle is a horizontal line so fill the row to x2, y2 and skip for loop
	if ( line1Slope == 0.0f || isnan(line1Slope) )
	{
		// fill row to x2, y2
		float tempX1 = x1FSorted;
		float tempY1 = y1FSorted;
		float tempX2 = x2FSorted;
		float tempY2 = y2FSorted;

		// if this 'triangle' is essentially just a straight line
		if ( y2FSorted == y3FSorted )
		{
			tempX2 = x3FSorted;
			tempY2 = y3FSorted;
		}

		// if after clipping this line exists within the screen, render the line
		if ( clipLine(&tempX1, &tempY1, &tempX2, &tempY2) )
		{
			int tempX1Int = tempX1 * (m_FBWidth  - 1);
			int tempY1Int = tempY1 * (m_FBHeight - 1);
			int tempX2Int = tempX2 * (m_FBWidth  - 1);
			int tempY2Int = tempY2 * (m_FBHeight - 1);

			int tempXY1 = ( (tempY1Int * m_FBWidth) + tempX1Int );
			int tempXY2 = ( (tempY2Int * m_FBWidth) + tempX2Int );

			for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );
			}
		}

		xRightAccumulator = (float) x2Sorted;
	}
	else
	{
		// render up until the second vertice
		for (int row = y1Sorted; row <= y2Sorted; row++)
		{
			// clip vertically if row is off screen
			if (row >= (int)m_FBHeight)
			{
				break;
			}
			else if (row >= 0)
			{
				// rounding the points and clipping horizontally
				unsigned int leftX  = std::min( std::max((int)std::round(xLeftAccumulator), 0), (int)m_FBWidth - 1 );
				unsigned int rightX = std::max( std::min((int)std::round(xRightAccumulator), (int)m_FBWidth - 1), 0 );

				unsigned int tempXY1 = ( (row * m_FBWidth) + leftX  );
				unsigned int tempXY2 = ( (row * m_FBWidth) + rightX );

				for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
				{
					m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );
				}

				// increment accumulators
				xLeftAccumulator  += xLeftIncrTop;
				xRightAccumulator += xRightIncrTop;

				// to prevent xRightAccumulator from surpassing x2 and xLeftAccumulator from surpassing x2
				if ( !needsSwapping && x1Sorted > x2Sorted && y1Sorted < y2Sorted && xLeftAccumulator < x2Sorted )
				{
					xLeftAccumulator = x2Sorted;

					if ( y2Sorted == y3Sorted && xRightAccumulator > x3Sorted )
					{
						xRightAccumulator = x3Sorted;
					}
				}
				else if ( needsSwapping && x1Sorted < x2Sorted && y1Sorted < y2Sorted && xRightAccumulator > x2Sorted )
				{
					xRightAccumulator = x2Sorted;
				}
			}
			else // even if off screen, we still need to increment xLeftAccumulator and xRightAccumulator
			{
				// increment accumulators
				xLeftAccumulator  += xLeftIncrTop;
				xRightAccumulator += xRightIncrTop;

				// to prevent xRightAccumulator from surpassing x2 and xLeftAccumulator from surpassing x2
				if ( !needsSwapping && x1Sorted > x2Sorted && y1Sorted < y2Sorted && xLeftAccumulator < x2Sorted )
				{
					xLeftAccumulator = x2Sorted;

					if ( y2Sorted == y3Sorted && xRightAccumulator > x3Sorted )
					{
						xRightAccumulator = x3Sorted;
					}
				}
				else if ( needsSwapping && x1Sorted < x2Sorted && y1Sorted < y2Sorted && xRightAccumulator > x2Sorted )
				{
					xRightAccumulator = x2Sorted;
				}
			}
		}
	}

skip:
	// rasterize up until the last vertice
	if (y2Sorted != y3Sorted) // if the bottom of the triangle isn't a horizontal line
	{
		for (int row = y2Sorted + 1; row <= y3Sorted; row++)
		{
			// clip vertically if row is off screen
			if (row >= (int)m_FBHeight)
			{
				break;
			}
			else if ( row >= 0 )
			{
				// rounding the points and clipping horizontally
				unsigned int leftX  = std::min( std::max((int)std::round(xLeftAccumulator), 0), (int)m_FBWidth - 1 );
				unsigned int rightX = std::max( std::min((int)std::round(xRightAccumulator), (int)m_FBWidth - 1), 0 );

				unsigned int tempXY1 = ( (row * m_FBWidth) + leftX  );
				unsigned int tempXY2 = ( (row * m_FBWidth) + rightX );

				for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
				{
					m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );
				}

				// increment accumulators
				xLeftAccumulator  += xLeftIncrBottom;
				xRightAccumulator += xRightIncrBottom;
			}
		}
	}
}
static inline float triGradNormalizedDist (float currentDistFromXY1, float endDistFromXY1)
{
	return std::max( 1.0f - (currentDistFromXY1 / endDistFromXY1), 0.0f );
}

static inline void calcTriGradients( float& rStartEdgeDistT, float& rEndEdgeDistT, float& rStartEdgeDistB, float& rEndEdgeDistB,
					float& gStartEdgeDistT, float& gEndEdgeDistT, float& gStartEdgeDistB, float& gEndEdgeDistB,
					float& bStartEdgeDistT, float &bEndEdgeDistT, float& bStartEdgeDistB, float& bEndEdgeDistB,
					const float& xy1DistToXy2, const float& xy1DistToXy3, const float& xy2DistToXy3, const int& y1UInt,
					const int& y1Sorted, const int& y2Sorted, const int &y3Sorted, const int& x1Int, const int& x2Int,
					const int& x3Int, const int& y2Int, const int& y3Int, const bool& needsSwapping, const float& zeroVal )
{
	if ( y1UInt == y1Sorted &&
		(y1Sorted < y2Sorted || (y1Sorted == y2Sorted && x1Int < x2Int)) ) // xy1 is on top
	{
		// first assuming that xy2 is above and to the left of xy3
		rStartEdgeDistT = xy1DistToXy2;
		rEndEdgeDistT = xy1DistToXy3;
		gStartEdgeDistT = xy1DistToXy2;
		bEndEdgeDistT = xy1DistToXy3;
		rStartEdgeDistB = xy1DistToXy2;
		rEndEdgeDistB = xy1DistToXy3;
		gStartEdgeDistB = xy2DistToXy3;
		bStartEdgeDistB = xy2DistToXy3;
		bEndEdgeDistB = xy1DistToXy3;

		if ( x3Int < x2Int )
		{
			rStartEdgeDistB = zeroVal;
			rEndEdgeDistB = xy1DistToXy3;

			if ( y3Int < y2Int )
			{
				if ( needsSwapping )
				{
					rStartEdgeDistB = xy1DistToXy2;
					rEndEdgeDistB = zeroVal;
					gStartEdgeDistB = xy1DistToXy2;
					gEndEdgeDistB = xy2DistToXy3;
					bStartEdgeDistB = zeroVal;
					bEndEdgeDistB = xy2DistToXy3;
				}
				else
				{
					rStartEdgeDistT = xy1DistToXy3;
					rEndEdgeDistT = xy1DistToXy2;
					rStartEdgeDistB = zeroVal;
					rEndEdgeDistB = xy1DistToXy2;
					gStartEdgeDistT = zeroVal;
					gEndEdgeDistT = xy1DistToXy2;
					gEndEdgeDistB = xy1DistToXy2;
					bStartEdgeDistT = xy1DistToXy3;
					bEndEdgeDistT = zeroVal;
					bStartEdgeDistB = xy2DistToXy3;
					bEndEdgeDistB = zeroVal;
				}
			}
			else if ( needsSwapping )
			{
				rStartEdgeDistT = xy1DistToXy3;
				rEndEdgeDistT = xy1DistToXy2;
				rStartEdgeDistB = xy1DistToXy3;
				rEndEdgeDistB = zeroVal;
				gStartEdgeDistT = zeroVal;
				gEndEdgeDistT = xy1DistToXy2;
				gStartEdgeDistB = zeroVal;
				gEndEdgeDistB = xy2DistToXy3;
				bStartEdgeDistT = xy1DistToXy3;
				bEndEdgeDistT = zeroVal;
				bStartEdgeDistB = xy1DistToXy3;
				bEndEdgeDistB = xy2DistToXy3;
			}
		}
		else if ( y3Int < y2Int )
		{
			if ( needsSwapping )
			{
				gStartEdgeDistB = xy1DistToXy2;
				gEndEdgeDistB = xy2DistToXy3;
				bStartEdgeDistB = zeroVal;
				bEndEdgeDistB = xy2DistToXy3;
			}
			else
			{
				rStartEdgeDistT = xy1DistToXy3;
				rEndEdgeDistT = xy1DistToXy2;
				rStartEdgeDistB = zeroVal;
				rEndEdgeDistB = xy1DistToXy2;
				gStartEdgeDistT = zeroVal;
				gEndEdgeDistT = xy1DistToXy2;
				gStartEdgeDistB = xy2DistToXy3;
				gEndEdgeDistB = xy1DistToXy2;
				bStartEdgeDistT = xy1DistToXy3;
				bEndEdgeDistT = zeroVal;
				bStartEdgeDistB = xy2DistToXy3;
				bEndEdgeDistB = zeroVal;
			}
		}
		else if ( needsSwapping )
		{
			rStartEdgeDistT = xy1DistToXy3;
			rEndEdgeDistT = xy1DistToXy2;
			rStartEdgeDistB = xy1DistToXy3;
			rEndEdgeDistB = zeroVal;
			gStartEdgeDistT = zeroVal;
			gEndEdgeDistT = xy1DistToXy2;
			gStartEdgeDistB = zeroVal;
			gEndEdgeDistB = xy2DistToXy3;
			bStartEdgeDistT = xy1DistToXy3;
			bEndEdgeDistT = zeroVal;
			bStartEdgeDistB = xy1DistToXy3;
			bEndEdgeDistB = xy2DistToXy3;
		}
	}
	else if ( y1UInt == y2Sorted &&
		(y2Sorted < y3Sorted || (y2Sorted == y3Sorted && x1Int < x2Int)) ) // xy1 is in the middle
	{
		// first assuming that xy2 is above and to the left of xy3
		rStartEdgeDistT = xy1DistToXy2;
		gStartEdgeDistT = xy1DistToXy2;
		gEndEdgeDistT = xy2DistToXy3;
		bEndEdgeDistT = xy2DistToXy3;
		rStartEdgeDistB = xy1DistToXy3;
		gEndEdgeDistB = xy2DistToXy3;
		bStartEdgeDistB = xy1DistToXy3;
		bEndEdgeDistB = xy2DistToXy3;

		if ( x3Int < x2Int )
		{
			if ( y3Int < y2Int )
			{
				if ( needsSwapping )
				{
					rStartEdgeDistT = zeroVal;
					rEndEdgeDistT = xy1DistToXy3;
					rStartEdgeDistB = zeroVal;
					rEndEdgeDistB = xy1DistToXy2;
					gStartEdgeDistT = xy2DistToXy3;
					gEndEdgeDistT = zeroVal;
					gStartEdgeDistB = xy2DistToXy3;
					gEndEdgeDistB = xy1DistToXy2;
					bStartEdgeDistT = xy2DistToXy3;
					bEndEdgeDistT = xy1DistToXy3;
					bStartEdgeDistB = xy2DistToXy3;
					bEndEdgeDistB = zeroVal;
				}
				else
				{
					rStartEdgeDistT = xy1DistToXy3;
					rStartEdgeDistB = xy1DistToXy2;
					gStartEdgeDistB = xy1DistToXy2;
					bStartEdgeDistT = xy1DistToXy3;
					bStartEdgeDistB = zeroVal;
					bEndEdgeDistB = xy2DistToXy3;
				}
			}
			else if ( needsSwapping )
			{
				rStartEdgeDistT = zeroVal;
				rEndEdgeDistT = xy1DistToXy2;
				rStartEdgeDistB = zeroVal;
				rEndEdgeDistB = xy1DistToXy3;
				gStartEdgeDistT = xy2DistToXy3;
				gEndEdgeDistT = xy1DistToXy2;
				gStartEdgeDistB = xy2DistToXy3;
				gEndEdgeDistB = zeroVal;
				bStartEdgeDistT = xy2DistToXy3;
				bEndEdgeDistT = zeroVal;
				bStartEdgeDistB = xy2DistToXy3;
				bEndEdgeDistB = xy1DistToXy3;
			}
		}
		else if ( y3Int < y2Int )
		{
			if ( needsSwapping )
			{
				rStartEdgeDistT = zeroVal;
				rEndEdgeDistT = xy1DistToXy3;
				rStartEdgeDistB = zeroVal;
				rEndEdgeDistB = xy1DistToXy2;
				gStartEdgeDistT = xy2DistToXy3;
				gEndEdgeDistT = zeroVal;
				gStartEdgeDistB = xy2DistToXy3;
				gEndEdgeDistB = xy1DistToXy2;
				bStartEdgeDistT = xy2DistToXy3;
				bEndEdgeDistT = xy1DistToXy3;
				bStartEdgeDistB = xy2DistToXy3;
				bEndEdgeDistB = zeroVal;
			}
			else
			{
				rStartEdgeDistT = xy1DistToXy3;
				rStartEdgeDistB = xy1DistToXy2;
				gStartEdgeDistB = xy1DistToXy2;
				bStartEdgeDistT = xy1DistToXy3;
				bStartEdgeDistB = zeroVal;
			}
		}
		else if ( needsSwapping )
		{
			rStartEdgeDistT = zeroVal;
			rEndEdgeDistT = xy1DistToXy2;
			gStartEdgeDistT = xy2DistToXy3;
			gEndEdgeDistT = xy1DistToXy2;
			bStartEdgeDistT = xy2DistToXy3;
			bEndEdgeDistT = zeroVal;
			rStartEdgeDistB = zeroVal;
			rEndEdgeDistB = xy1DistToXy3;
			gStartEdgeDistB = xy2DistToXy3;
			gEndEdgeDistB = zeroVal;
			bStartEdgeDistB = xy2DistToXy3;
			bEndEdgeDistB = xy1DistToXy3;
		}
	}
	else if ( y1UInt == y3Sorted ) // xy1 is on the bottom
	{
		// first assuming that xy2 is above and to the left of xy3
		rEndEdgeDistT = xy1DistToXy2;
		rStartEdgeDistB = xy1DistToXy3;
		rEndEdgeDistB = xy1DistToXy2;
		gStartEdgeDistT = xy2DistToXy3;
		gEndEdgeDistT = xy1DistToXy2;
		gEndEdgeDistB = xy1DistToXy2;
		bStartEdgeDistT = xy2DistToXy3;
		bStartEdgeDistB = xy1DistToXy3;

		if ( x3Int < x2Int )
		{
			if ( y3Int <= y2Int )
			{
				if ( needsSwapping )
				{
					rStartEdgeDistT = xy1DistToXy3;
					rEndEdgeDistT = zeroVal;
					gStartEdgeDistT = zeroVal;
					gEndEdgeDistT = xy2DistToXy3;
					gStartEdgeDistB = zeroVal;
					bStartEdgeDistT = xy1DistToXy3;
					bEndEdgeDistT = xy2DistToXy3;
					bEndEdgeDistB = zeroVal;
				}
				else
				{
					rEndEdgeDistT = xy1DistToXy3;
					rStartEdgeDistB = xy1DistToXy2;
					rEndEdgeDistB = xy1DistToXy3;
					gEndEdgeDistT = zeroVal;
					gStartEdgeDistB = xy1DistToXy2;
					gEndEdgeDistB = zeroVal;
					bEndEdgeDistT = xy1DistToXy3;
					bStartEdgeDistB = zeroVal;
					bEndEdgeDistB = xy1DistToXy3;
				}
			}
			else if ( needsSwapping )
			{
				rStartEdgeDistT = xy1DistToXy2;
				rEndEdgeDistT = zeroVal;
				rStartEdgeDistB = xy1DistToXy2;
				rEndEdgeDistB = xy1DistToXy3;
				gStartEdgeDistT = xy1DistToXy2;
				gEndEdgeDistT = xy2DistToXy3;
				gStartEdgeDistB = xy1DistToXy2;
				gEndEdgeDistB = zeroVal;
				bStartEdgeDistT = zeroVal;
				bEndEdgeDistT = xy2DistToXy3;
				bStartEdgeDistB = zeroVal;
				bEndEdgeDistB = xy1DistToXy3;
			}
		}
		else if ( y3Int < y2Int )
		{
			if ( needsSwapping )
			{
				rStartEdgeDistT = xy1DistToXy3;
				rEndEdgeDistT = zeroVal;
				gStartEdgeDistT = zeroVal;
				gEndEdgeDistT = xy2DistToXy3;
				bStartEdgeDistT = xy1DistToXy3;
				bEndEdgeDistT = xy2DistToXy3;
				bStartEdgeDistB = xy1DistToXy3;
				bEndEdgeDistB = zeroVal;
			}
			else
			{
				gEndEdgeDistT = zeroVal;
				gStartEdgeDistB = xy1DistToXy2;
				gEndEdgeDistB = zeroVal;
				bEndEdgeDistT = xy1DistToXy3;
				bStartEdgeDistB = zeroVal;
				bEndEdgeDistB = xy1DistToXy3;
			}
		}
		else if ( needsSwapping )
		{
			rStartEdgeDistT = xy1DistToXy2;
			rEndEdgeDistT = zeroVal;
			rStartEdgeDistB = xy1DistToXy2;
			rEndEdgeDistB = xy1DistToXy3;
			gStartEdgeDistT = xy1DistToXy2;
			gEndEdgeDistT = xy2DistToXy3;
			gStartEdgeDistB = xy1DistToXy2;
			gEndEdgeDistB = zeroVal;
			bStartEdgeDistT = zeroVal;
			bEndEdgeDistT = xy2DistToXy3;
			bStartEdgeDistB = zeroVal;
			bEndEdgeDistB = xy1DistToXy3;
		}
	}
}

void SoftwareGraphics::drawTriangleGradient (float x1, float y1, float x2, float y2, float x3, float y3)
{
	// get previous color, since we'll want to set it back when we're done with the gradient colors
	const Color previousColor = m_ColorProfile->getColor();

	// getting the pixel values of the vertices
	int x1Int = x1 * (m_FBWidth  - 1);
	int y1Int = y1 * (m_FBHeight - 1);
	int x2Int = x2 * (m_FBWidth  - 1);
	int y2Int = y2 * (m_FBHeight - 1);
	int x3Int = x3 * (m_FBWidth  - 1);
	int y3Int = y3 * (m_FBHeight - 1);

	int x1Sorted = x1Int;
	int y1Sorted = y1Int;
	int x2Sorted = x2Int;
	int y2Sorted = y2Int;
	int x3Sorted = x3Int;
	int y3Sorted = y3Int;
	float x1FSorted = x1;
	float y1FSorted = y1;
	float x2FSorted = x2;
	float y2FSorted = y2;
	float x3FSorted = x3;
	float y3FSorted = y3;

	// sorting vertices
	triSortVertices( x1Sorted, y1Sorted, x1FSorted, y1FSorted, x2Sorted, y2Sorted, x2FSorted, y2FSorted,
						x3Sorted, y3Sorted, x3FSorted, y3FSorted );

	// getting the slope of each line
	float line1Slope = ((float) y2Sorted - y1Sorted) / ((float) x2Sorted - x1Sorted);
	float line2Slope = ((float) y3Sorted - y1Sorted) / ((float) x3Sorted - x1Sorted);
	float line3Slope = ((float) y3Sorted - y2Sorted) / ((float) x3Sorted - x2Sorted);

	// floats for x-intercepts (assuming the top of the triangle is pointed for now)
	float xLeftAccumulator  = (float) x1Sorted;
	float xRightAccumulator = (float) x1Sorted;

	// floats for incrementing xLeftAccumulator and xRightAccumulator
	float xLeftIncrTop     = 1.0f / line1Slope;
	float xRightIncrTop    = 1.0f / line2Slope;
	float xLeftIncrBottom  = 1.0f / line3Slope;
	float xRightIncrBottom = 1.0f / line2Slope;

	// xLeftIncrBottom < xRightIncrBottom is a substitute for line2Slope being on the top or bottom
	bool needsSwapping = (xLeftIncrBottom < xRightIncrBottom);

	// depending on the position of the vertices, we need to swap increments
	if ( (needsSwapping && x1Sorted < x2Sorted) || (needsSwapping && x1Sorted >= x2Sorted && x2Sorted > x3Sorted) )
	{
		float tempIncr = xLeftIncrTop;
		xLeftIncrTop = xRightIncrTop;
		xRightIncrTop = tempIncr;

		tempIncr = xLeftIncrBottom;
		xLeftIncrBottom = xRightIncrBottom;
		xRightIncrBottom = tempIncr;
	}

	// coordinates variables
	int xInRelationLeftmost = (x1Int <= x2Int) ? x1Int : x2Int;
	xInRelationLeftmost = (xInRelationLeftmost <= x3Int) ? xInRelationLeftmost : x3Int;
	int xInRelationRightmost = (x1Int >= x2Int) ? x1Int : x2Int;
	xInRelationRightmost = (xInRelationRightmost >= x3Int) ? xInRelationRightmost : x3Int;
	float xInRelationIncr = 1.0f / (xInRelationRightmost - xInRelationLeftmost);
	float yInRelationIncr = 1.0f / (y3Sorted - y1Sorted);
	float x1InRelationToXLeft = 1.0f - ((xInRelationRightmost - x1Int) * xInRelationIncr);
	float x2InRelationToX1 = -1.0f * ((x1Int - x2Int) * xInRelationIncr);
	float y2InRelationToY1 = -1.0f * ((y1Int - y2Int) * yInRelationIncr);
	float x3InRelationToX1 = -1.0f * ((x1Int - x3Int) * xInRelationIncr);
	float y3InRelationToY1 = -1.0f * ((y1Int - y3Int) * yInRelationIncr);
	y2InRelationToY1 = ( isnan(y2InRelationToY1) ) ? 0.0f : y2InRelationToY1; // can happen when
	y3InRelationToY1 = ( isnan(y3InRelationToY1) ) ? 0.0f : y3InRelationToY1; // y1 = y2 = y3
	float xy1DistToXy2 = this->distance(0.0f, 0.0f, x2InRelationToX1, y2InRelationToY1);
	float xy1DistToXy3 = this->distance(0.0f, 0.0f, x3InRelationToX1, y3InRelationToY1);
	float xy2DistToXy3 = this->distance(x2InRelationToX1, y2InRelationToY1, x3InRelationToX1, y3InRelationToY1);
	// setting these values to std::numeric_limits<float>::min() will result in triGradNormalizedDist returning 0.0f
	const float zeroVal = std::numeric_limits<float>::min();
	float rStartEdgeDistT = zeroVal; // T for rendering the top half of the triangle
	float gStartEdgeDistT = zeroVal;
	float bStartEdgeDistT = zeroVal;
	float rEndEdgeDistT = zeroVal;
	float gEndEdgeDistT = zeroVal;
	float bEndEdgeDistT = zeroVal;
	float rStartEdgeDistB = zeroVal; // B for rendering the bottom half of the triangle
	float gStartEdgeDistB = zeroVal;
	float bStartEdgeDistB = zeroVal;
	float rEndEdgeDistB = zeroVal;
	float gEndEdgeDistB = zeroVal;
	float bEndEdgeDistB = zeroVal;
	// swap start and end values based on vertex positions
	calcTriGradients( rStartEdgeDistT, rEndEdgeDistT, rStartEdgeDistB, rEndEdgeDistB, gStartEdgeDistT, gEndEdgeDistT,
						gStartEdgeDistB, gEndEdgeDistB, bStartEdgeDistT, bEndEdgeDistT, bStartEdgeDistB, bEndEdgeDistB,
						xy1DistToXy2, xy1DistToXy3, xy2DistToXy3, y1Int, y1Sorted, y2Sorted, y3Sorted, x1Int, x2Int,
						x3Int, y2Int, y3Int, needsSwapping, zeroVal );

	// setting the y value for gradients
	float yInRelationToY1 = -1.0f * (1.0f - ((y3Sorted - y1Int) * yInRelationIncr));
	yInRelationToY1 = ( isnan(yInRelationToY1) ) ? 0.0f : yInRelationToY1;

	// if slope is zero, the top of the triangle is a horizontal line so fill the row to x2, y2 and skip for loop
	if ( line1Slope == 0.0f || isnan(line1Slope) )
	{
		// fill row to x2, y2
		float tempX1 = x1FSorted;
		float tempY1 = y1FSorted;
		float tempX2 = x2FSorted;
		float tempY2 = y2FSorted;

		// if this 'triangle' is essentially just a straight line
		if ( y2FSorted == y3FSorted )
		{
			tempX2 = x3FSorted;
			tempY2 = y3FSorted;
		}

		// if after clipping this line exists within the screen, render the line
		if ( clipLine(&tempX1, &tempY1, &tempX2, &tempY2) )
		{
			int tempX1Int = tempX1 * (m_FBWidth  - 1);
			int tempY1Int = tempY1 * (m_FBHeight - 1);
			int tempX2Int = tempX2 * (m_FBWidth  - 1);
			int tempY2Int = tempY2 * (m_FBHeight - 1);

			int tempXY1 = ( (tempY1Int * m_FBWidth) + tempX1Int );
			int tempXY2 = ( (tempY2Int * m_FBWidth) + tempX2Int );

			// setting the x values for gradients
			float xLeftInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - tempX1Int) * xInRelationIncr);
			float xRightInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - tempX2Int) * xInRelationIncr);
			// get important distances
			float xy1DistToXLeft = this->distance(0.0f, 0.0f, xLeftInRelationToX1, yInRelationToY1);
			float xy1DistToXRight = this->distance(0.0f, 0.0f, xRightInRelationToX1, yInRelationToY1);
			float xy2DistToXLeft = this->distance(x2InRelationToX1, y2InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
			float xy2DistToXRight = this->distance(x2InRelationToX1, y2InRelationToY1, xRightInRelationToX1, yInRelationToY1);
			float xy3DistToXLeft = this->distance(x3InRelationToX1, y3InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
			float xy3DistToXRight = this->distance(x3InRelationToX1, y3InRelationToY1, xRightInRelationToX1, yInRelationToY1);
			// compute the starting and ending color values for each scanline
			float rStart = triGradNormalizedDist(xy1DistToXLeft, rStartEdgeDistT);
			float gStart = triGradNormalizedDist(xy2DistToXLeft, gStartEdgeDistT);
			float bStart = triGradNormalizedDist(xy3DistToXLeft, bStartEdgeDistT);
			float rEnd = triGradNormalizedDist(xy1DistToXRight, rEndEdgeDistT);
			float gEnd = triGradNormalizedDist(xy2DistToXRight, gEndEdgeDistT);
			float bEnd = triGradNormalizedDist(xy3DistToXRight, bEndEdgeDistT);
			// linearly interpolate between the two values
			float rIncr = (rEnd - rStart) / (tempX2Int - tempX1Int);
			float rCurrent = rStart;
			float gIncr = (gEnd - gStart) / (tempX2Int - tempX1Int);
			float gCurrent = gStart;
			float bIncr = (bEnd - bStart) / (tempX2Int - tempX1Int);
			float bCurrent = bStart;

			for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
			{
				m_ColorProfile->setColor( rCurrent, gCurrent, bCurrent );
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

				rCurrent += rIncr;
				gCurrent += gIncr;
				bCurrent += bIncr;
			}
		}

		xRightAccumulator = (float) x2Sorted;
		yInRelationToY1 += yInRelationIncr;
	}
	else
	{
		// render up until the second vertice
		for (int row = y1Sorted; row <= y2Sorted; row++)
		{
			// clip vertically if row is off screen
			if (row >= (int)m_FBHeight)
			{
				break;
			}
			else if (row >= 0)
			{
				// rounding the points and clipping horizontally
				unsigned int leftX  = std::min( std::max((int)std::round(xLeftAccumulator), 0), (int)m_FBWidth - 1 );
				unsigned int rightX = std::max( std::min((int)std::round(xRightAccumulator), (int)m_FBWidth - 1), 0 );

				unsigned int tempXY1 = ( (row * m_FBWidth) + leftX  );
				unsigned int tempXY2 = ( (row * m_FBWidth) + rightX );

				// setting the x values for gradients
				float xLeftInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - leftX) * xInRelationIncr);
				float xRightInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - rightX) * xInRelationIncr);
				// get important distances
				float xy1DistToXLeft = this->distance(0.0f, 0.0f, xLeftInRelationToX1, yInRelationToY1);
				float xy1DistToXRight = this->distance(0.0f, 0.0f, xRightInRelationToX1, yInRelationToY1);
				float xy2DistToXLeft = this->distance(x2InRelationToX1, y2InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
				float xy2DistToXRight = this->distance(x2InRelationToX1, y2InRelationToY1, xRightInRelationToX1, yInRelationToY1);
				float xy3DistToXLeft = this->distance(x3InRelationToX1, y3InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
				float xy3DistToXRight = this->distance(x3InRelationToX1, y3InRelationToY1, xRightInRelationToX1, yInRelationToY1);
				// compute the starting and ending color values for each scanline
				float rStart = triGradNormalizedDist(xy1DistToXLeft, rStartEdgeDistT);
				float gStart = triGradNormalizedDist(xy2DistToXLeft, gStartEdgeDistT);
				float bStart = triGradNormalizedDist(xy3DistToXLeft, bStartEdgeDistT);
				float rEnd = triGradNormalizedDist(xy1DistToXRight, rEndEdgeDistT);
				float gEnd = triGradNormalizedDist(xy2DistToXRight, gEndEdgeDistT);
				float bEnd = triGradNormalizedDist(xy3DistToXRight, bEndEdgeDistT);
				// linearly interpolate between the two values
				float rIncr = (rEnd - rStart) / (rightX - leftX);
				float rCurrent = rStart;
				float gIncr = (gEnd - gStart) / (rightX - leftX);
				float gCurrent = gStart;
				float bIncr = (bEnd - bStart) / (rightX - leftX);
				float bCurrent = bStart;

				for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
				{
					m_ColorProfile->setColor( rCurrent, gCurrent, bCurrent );
					m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

					rCurrent += rIncr;
					gCurrent += gIncr;
					bCurrent += bIncr;
				}

				// increment accumulators
				xLeftAccumulator  += xLeftIncrTop;
				xRightAccumulator += xRightIncrTop;

				// to prevent xRightAccumulator from surpassing x2 and xLeftAccumulator from surpassing x2
				if ( !needsSwapping && x1Sorted > x2Sorted && y1Sorted < y2Sorted && xLeftAccumulator < x2Sorted )
				{
					xLeftAccumulator = x2Sorted;

					if ( y2Sorted == y3Sorted && xRightAccumulator > x3Sorted )
					{
						xRightAccumulator = x3Sorted;
					}
				}
				else if ( needsSwapping && x1Sorted < x2Sorted && y1Sorted < y2Sorted && xRightAccumulator > x2Sorted )
				{
					xRightAccumulator = x2Sorted;
				}
			}
			else // even if off screen, we still need to increment xLeftAccumulator and xRightAccumulator
			{
				// increment accumulators
				xLeftAccumulator  += xLeftIncrTop;
				xRightAccumulator += xRightIncrTop;

				// to prevent xRightAccumulator from surpassing x2 and xLeftAccumulator from surpassing x2
				if ( !needsSwapping && x1Sorted > x2Sorted && y1Sorted < y2Sorted && xLeftAccumulator < x2Sorted )
				{
					xLeftAccumulator = x2Sorted;

					if ( y2Sorted == y3Sorted && xRightAccumulator > x3Sorted )
					{
						xRightAccumulator = x3Sorted;
					}
				}
				else if ( needsSwapping && x1Sorted < x2Sorted && y1Sorted < y2Sorted && xRightAccumulator > x2Sorted )
				{
					xRightAccumulator = x2Sorted;
				}
			}

			yInRelationToY1 += yInRelationIncr;
		}
	}

skip:
	// rasterize up until the last vertice
	if (y2Sorted != y3Sorted) // if the bottom of the triangle isn't a horizontal line
	{
		for (int row = y2Sorted + 1; row <= y3Sorted; row++)
		{
			// clip vertically if row is off screen
			if (row >= (int)m_FBHeight)
			{
				break;
			}
			else if ( row >= 0 )
			{
				// rounding the points and clipping horizontally
				unsigned int leftX  = std::min( std::max((int)std::round(xLeftAccumulator), 0), (int)m_FBWidth - 1 );
				unsigned int rightX = std::max( std::min((int)std::round(xRightAccumulator), (int)m_FBWidth - 1), 0 );

				unsigned int tempXY1 = ( (row * m_FBWidth) + leftX  );
				unsigned int tempXY2 = ( (row * m_FBWidth) + rightX );

				// setting the x values for gradients
				float xLeftInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - leftX) * xInRelationIncr);
				float xRightInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - rightX) * xInRelationIncr);
				// get important distances
				float xy1DistToXLeft = this->distance(0.0f, 0.0f, xLeftInRelationToX1, yInRelationToY1);
				float xy1DistToXRight = this->distance(0.0f, 0.0f, xRightInRelationToX1, yInRelationToY1);
				float xy2DistToXLeft = this->distance(x2InRelationToX1, y2InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
				float xy2DistToXRight = this->distance(x2InRelationToX1, y2InRelationToY1, xRightInRelationToX1, yInRelationToY1);
				float xy3DistToXLeft = this->distance(x3InRelationToX1, y3InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
				float xy3DistToXRight = this->distance(x3InRelationToX1, y3InRelationToY1, xRightInRelationToX1, yInRelationToY1);
				// compute the starting and ending color values for each scanline
				float rStart = triGradNormalizedDist(xy1DistToXLeft, rStartEdgeDistB);
				float gStart = triGradNormalizedDist(xy2DistToXLeft, gStartEdgeDistB);
				float bStart = triGradNormalizedDist(xy3DistToXLeft, bStartEdgeDistB);
				float rEnd = triGradNormalizedDist(xy1DistToXRight, rEndEdgeDistB);
				float gEnd = triGradNormalizedDist(xy2DistToXRight, gEndEdgeDistB);
				float bEnd = triGradNormalizedDist(xy3DistToXRight, bEndEdgeDistB);
				// linearly interpolate between the two values
				float rIncr = (rEnd - rStart) / (rightX - leftX);
				float rCurrent = rStart;
				float gIncr = (gEnd - gStart) / (rightX - leftX);
				float gCurrent = gStart;
				float bIncr = (bEnd - bStart) / (rightX - leftX);
				float bCurrent = bStart;

				for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
				{
					m_ColorProfile->setColor( rCurrent, gCurrent, bCurrent );
					m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel );

					rCurrent += rIncr;
					gCurrent += gIncr;
					bCurrent += bIncr;
				}

				// increment accumulators
				xLeftAccumulator  += xLeftIncrBottom;
				xRightAccumulator += xRightIncrBottom;
			}

			yInRelationToY1 += yInRelationIncr;
		}
	}

	// set the previously used color back since we're done with the gradients
	m_ColorProfile->setColor( previousColor );
}

void SoftwareGraphics::drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x4, y4 );
	drawLine( x4, y4, x1, y1 );
}

void SoftwareGraphics::drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	drawTriangleFilled( x1, y1, x2, y2, x3, y3 );
	drawTriangleFilled( x1, y1, x4, y4, x3, y3 );
}

void SoftwareGraphics::drawQuadGradient (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	drawTriangleGradient( x1, y1, x2, y2, x3, y3 );
	drawTriangleGradient( x1, y1, x4, y4, x3, y3 );
}

void SoftwareGraphics::drawCircleHelper (int originX, int originY, int x, int y, bool filled)
{
	int x1_3 = originX + x;
	int y1_2 = originY + y;
	int x2_4 = originX - x;
	int y3_4 = originY - y;
	int x5_7 = originX + y;
	int y5_6 = originY + x;
	int x6_8 = originX - y;
	int y7_8 = originY - x;

	// clipping
	if (x1_3 < 0) x1_3 = 0;
	if (x2_4 < 0) x2_4 = 0;
	if (x5_7 < 0) x5_7 = 0;
	if (x6_8 < 0) x6_8 = 0;
	if (x1_3 > (int)m_FBWidth) x1_3 = m_FBWidth;
	if (x2_4 > (int)m_FBWidth) x2_4 = m_FBWidth;
	if (x5_7 > (int)m_FBWidth) x5_7 = m_FBWidth;
	if (x6_8 > (int)m_FBWidth) x6_8 = m_FBWidth;

	int pixel1 = ( (y1_2 * m_FBWidth) + x1_3  );
	int pixel2 = ( (y1_2 * m_FBWidth) + x2_4  );
	int pixel3 = ( (y3_4 * m_FBWidth) + x1_3  );
	int pixel4 = ( (y3_4 * m_FBWidth) + x2_4  );
	int pixel5 = ( (y5_6 * m_FBWidth) + x5_7  );
	int pixel6 = ( (y5_6 * m_FBWidth) + x6_8  );
	int pixel7 = ( (y7_8 * m_FBWidth) + x5_7  );
	int pixel8 = ( (y7_8 * m_FBWidth) + x6_8  );

	if (filled)
	{
		// span from pixel2 to pixel1
		int tempPixel = pixel2;
		if (y1_2 >= 0 && y1_2 < (int)m_FBHeight)
		{
			while (tempPixel < pixel1)
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, tempPixel );
				tempPixel += 1;
			}
		}

		// span from pixel4 to pixel3
		if (y3_4 >= 0 && y3_4 < (int)m_FBHeight)
		{
			tempPixel = pixel4;
			while (tempPixel < pixel3)
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, tempPixel );
				tempPixel += 1;
			}
		}

		// span from pixel6 to pixel5
		if (y5_6 >= 0 && y5_6 < (int)m_FBHeight)
		{
			tempPixel = pixel6;
			while (tempPixel < pixel5)
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, tempPixel );
				tempPixel += 1;
			}
		}

		// span form pixel8 to pixel7
		if (y7_8 >= 0 && y7_8 < (int)m_FBHeight)
		{
			tempPixel = pixel8;
			while (tempPixel < pixel7)
			{
				m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, tempPixel );
				tempPixel += 1;
			}
		}
	}
	else
	{
		if (y1_2 >= 0 && y1_2 < (int)m_FBHeight)
		{
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel1 );
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel2 );
		}
		if (y3_4 >= 0 && y3_4 < (int)m_FBHeight)
		{
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel3 );
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel4 );
		}
		if (y5_6 >= 0 && y5_6 < (int)m_FBHeight)
		{
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel5 );
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel6 );
		}
		if (y7_8 >= 0 && y7_8 < (int)m_FBHeight)
		{
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel7 );
			m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixel8 );
		}
	}
}

void SoftwareGraphics::drawCircle (float originX, float originY, float radius)
{
	// getting the pixel values of the vertices
	int originXUInt = originX * (m_FBWidth  - 1);
	int originYUInt = originY * (m_FBHeight - 1);
	int radiusUInt = radius * (m_FBWidth - 1);

	// bresenham's algorithm
	int x = 0;
	int y = radiusUInt;
	int decision = 3 - 2 * radiusUInt;

	drawCircleHelper( originXUInt, originYUInt, x, y );

	while (y >= x)
	{
		x++;
		if (decision > 0)
		{
			y--;
			decision = decision + 4 * (x - y) + 10;
		}
		else
		{
			decision = decision + 4 * x + 6;
		}

		drawCircleHelper( originXUInt, originYUInt, x, y );
	}
}



void SoftwareGraphics::drawCircleFilled (float originX, float originY, float radius)
{
	// getting the pixel values of the vertices
	int originXUInt = originX * (m_FBWidth  - 1);
	int originYUInt = originY * (m_FBHeight - 1);
	int radiusUInt = radius * (m_FBWidth - 1);

	// bresenham's algorithm
	int x = 0;
	int y = radiusUInt;
	int decision = 3 - 2 * radiusUInt;

	drawCircleHelper( originXUInt, originYUInt, x, y, true );

	while (y >= x)
	{
		x++;
		if (decision > 0)
		{
			y--;
			decision = decision + 4 * (x - y) + 10;
		}
		else
		{
			decision = decision + 4 * x + 6;
		}

		drawCircleHelper( originXUInt, originYUInt, x, y, true );
	}
}

void SoftwareGraphics::drawText (float xStart, float yStart, const char* text, float scaleFactor)
{
	// TODO text doesn't render if scale factor isn't an integer beyond 1.0f, fix later?
	if ( scaleFactor > 1.0f )
	{
		scaleFactor = std::round( scaleFactor );
	}

	// getting the pixel values of the vertices
	int currentXInt = xStart * (m_FBWidth  - 1);
	int currentYInt = yStart * (m_FBHeight - 1);
	int currentPixel = (currentYInt * m_FBWidth) + currentXInt;

	// widths and heights from the font
	unsigned int characterWidth = m_CurrentFont->getCharacterWidth();
	unsigned int characterHeight = m_CurrentFont->getBitmapHeight();
	unsigned int bitmapWidth = m_CurrentFont->getBitmapWidth();

	unsigned int bitmapRowTravel = 1;
	unsigned int bitmapPixelTravel = 1;
	if ( scaleFactor < 1.0f ) // we need to skip rows and columns if downscaling
	{
		bitmapRowTravel = std::floor( bitmapRowTravel / scaleFactor );
		bitmapPixelTravel = std::floor( bitmapPixelTravel / scaleFactor );
	}
	unsigned int scaledHeight = std::round( characterHeight * scaleFactor );
	unsigned int scaledWidth  = std::round( characterWidth * scaleFactor );
	float nNXTravel = static_cast<float>( scaledWidth ) / static_cast<float>( characterWidth ); // nearest neighbor scaling vars
	float nNYTravel = static_cast<float>( scaledHeight ) / static_cast<float>( characterHeight );
	float nNCurrentX = 0.0f; // these vars keep a 'running total' for upscaling
	float nNCurrentY = 0.0f;
	float nNYLeftOver = 0.0f;

	// for left border clipping
	unsigned int numXPixelsToSkip = 0;
	if ( xStart < 0.0f )
	{
		numXPixelsToSkip = std::abs( xStart ) * m_FBWidth;
	}

	// for right border clipping
	int rightBorderPixel = 0;
	if ( yStart >= 0.0f )
	{
		rightBorderPixel = m_FBWidth * ( std::floor(static_cast<float>(currentYInt * m_FBWidth) / m_FBWidth) + 1 );
	}
	else
	{
		rightBorderPixel = -1 * ( std::abs(currentYInt + 1) * m_FBWidth );
	}

	// go through each character and translate the pixels in the font to the frame buffer
	for( unsigned int charIndex = 0; text[charIndex] != '\0'; charIndex++ )
	{
		unsigned int charPixelIndex = m_CurrentFont->getCharacterIndex( text[charIndex] ) * characterWidth;
		unsigned int charByteIndex = std::floor( static_cast<float>(charPixelIndex) / 8.0f );
		unsigned int charPixelMask = 7 - ( charPixelIndex % 8 );

		for ( unsigned int row = 0; row < characterHeight; row += bitmapRowTravel )
		{
			unsigned int pixelsMovedRight = 0;
			unsigned int pixelsMovedDown = 0;
			unsigned int xPixelsSkipped = charIndex * scaledWidth;
			int rightClipX = 0;
			if ( scaleFactor >= 1.0f )
			{
				rightClipX = rightBorderPixel + ( m_FBWidth * std::floor(row * scaleFactor) );
			}
			else
			{
				rightClipX = rightBorderPixel + ( m_FBWidth * row );
			}

			for ( unsigned int pixel = 0; pixel < characterWidth; pixel += bitmapPixelTravel )
			{
				bool pixelOn = m_CurrentFont->getBitmapStart()[charByteIndex] & (1 << charPixelMask);

				while ( nNCurrentX < nNXTravel )
				{
					nNCurrentY = nNYLeftOver;
					pixelsMovedDown = 0;

					while ( nNCurrentY < nNYTravel )
					{
						int pixelToWrite = currentPixel + ( m_FBWidth * std::floor(nNCurrentY) );

						if ( pixelOn
								&& pixelToWrite >= 0
								&& pixelToWrite < static_cast<int>( m_FBWidth * m_FBHeight )
								&& xPixelsSkipped >= numXPixelsToSkip
								&& pixelToWrite < rightClipX )
						{
							m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixelToWrite );
						}

						rightClipX += m_FBWidth;
						nNCurrentY += 1.0f;
						pixelsMovedDown++;
					}

					rightClipX -= ( m_FBWidth * pixelsMovedDown );
					nNCurrentX += 1.0f;
					nNCurrentY = 0;
					currentPixel++;
					xPixelsSkipped++;
					pixelsMovedRight++;
				}

				nNYLeftOver = std::fmod( nNCurrentY, nNYTravel );
				nNCurrentX = std::fmod( nNCurrentX, nNXTravel );
				charPixelIndex++;
				charByteIndex = std::floor( static_cast<float>(charPixelIndex) / 8.0f );
				charPixelMask = 7 - ( charPixelIndex % 8 );
			}

			charPixelIndex += ( bitmapWidth - characterWidth );
			charPixelMask = 7 - ( charPixelIndex % 8 );
			charByteIndex = std::floor( static_cast<float>(charPixelIndex) / 8.0f );
			nNCurrentX = 0.0f;
			nNCurrentY = std::fmod( nNCurrentY, nNYTravel );
			currentPixel += ( (m_FBWidth * pixelsMovedDown) - pixelsMovedRight );
			pixelsMovedRight = 0;
		}

		// reset the y-index back to the top, move the x-index to the right of the last char
		currentYInt = yStart * (m_FBHeight - 1);
		currentXInt += static_cast<unsigned int>( characterWidth * scaleFactor );
		currentPixel = ( currentYInt * m_FBWidth ) + currentXInt;
	}
}

void SoftwareGraphics::drawSprite (float xStart, float yStart, Sprite& sprite)
{
	// getting the pixel values of the vertices
	int startXInt = xStart * (m_FBWidth  - 1);
	int startYInt = yStart * (m_FBWidth - 1);
	int currentXInt = startXInt;
	int currentYInt = startYInt;

	int spriteWidth = sprite.getWidth();
	int spriteHeight = sprite.getHeight();
	ColorProfile* spriteCP = sprite.getColorProfile();
	uint8_t* spritePixels = sprite.getPixels();

	float scaleFactor = sprite.getScaleFactor();

	float spriteRowTravel = 1.0f;
	float spritePixelTravel = 1.0f;
	if ( scaleFactor < 1.0f ) // we need to skip rows and columns if downscaling
	{
		spriteRowTravel = spriteRowTravel / scaleFactor;
		spritePixelTravel = spritePixelTravel / scaleFactor;
	}
	unsigned int scaledHeight = sprite.getScaledHeight();
	unsigned int scaledWidth  = sprite.getScaledWidth();
	float nNXTravel = static_cast<float>( scaledWidth ) / static_cast<float>( spriteWidth ); // nearest neighbor scaling vars
	float nNYTravel = static_cast<float>( scaledHeight ) / static_cast<float>( spriteHeight );
	float nNCurrentX = 0.0f; // these vars keep a 'running total' for upscaling
	float nNCurrentY = 0.0f;
	float nNYLeftOver = 0.0f;

	// for rotation
	int spriteRotDegrees = sprite.getRotationAngle();
	int spriteRotPointX = std::round( sprite.getRotationPointX() * scaleFactor );;
	int spriteRotPointY = std::round( sprite.getRotationPointY() * scaleFactor );

	// for bottom clipping
	int fbSize = m_FBWidth * m_FBHeight;

	for ( float row = 0; row < spriteHeight; row += spriteRowTravel )
	{
		unsigned int pixelsMovedRight = 0;
		unsigned int pixelsMovedDown = 0;
		unsigned int xPixelsSkipped = 0;

		for ( float pixel = 0; pixel < spriteWidth; pixel += spritePixelTravel )
		{
			Color color = spriteCP->getPixel( spritePixels, m_FBNumPixels, (std::floor(row) * spriteWidth) + std::floor(pixel) );

			while ( nNCurrentX < nNXTravel )
			{
				pixelsMovedDown = 0;
				nNCurrentY = nNYLeftOver;

				// do rotation
				float radians = (spriteRotDegrees * M_PI) / 180.0f;
				float sinVal = sin( radians );
				float cosVal = cos( radians );

				int newY = currentYInt;

				while ( nNCurrentY < nNYTravel )
				{
					int translationPointX = startXInt + spriteRotPointX;
					int translationPointY = startYInt + spriteRotPointY;
					int xTranslated = currentXInt - translationPointX;
					int yTranslated = newY - translationPointY;
					int xRotated = static_cast<int>( (+xTranslated * cosVal) - (yTranslated * sinVal) );
					int yRotated = static_cast<int>( (+xTranslated * sinVal) + (yTranslated * cosVal) );
					int xTranslatedBack = xRotated + translationPointX;
					int yTranslatedBack = yRotated + translationPointY;

					int pixelToWrite = (yTranslatedBack * m_FBWidth) + xTranslatedBack;

					if ( pixelToWrite >= 0 &&  // top clipping
							pixelToWrite < fbSize && // bottom clipping
							xTranslatedBack >= 0 && // left clipping
							xTranslatedBack < m_FBWidth && // right clipping
							! (color.m_IsMonochrome && color.m_M == 0.0f) )
					{
							m_ColorProfile->setColor( color );
							m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, pixelToWrite );
					}

					// TODO this is a smoothbrain way to remove the 'aliasing' that occurs when rotating
					// but it works,.. maybe fix later?
					// TODO update: commenting out for now because of how outrageously smoothbrain it is
					/*
					int sbPixelRight = pixelToWrite + 1;
					if ( sbPixelRight >= 0 &&  // top clipping
							sbPixelRight < fbSize && // bottom clipping
							xTranslatedBack + 1 >= 0 && // left clipping
							xTranslatedBack + 1 < m_FBWidth && // right clipping
							! (color.m_IsMonochrome && color.m_M == 0.0f) )
					{
							m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, sbPixelRight );
					}
					int sbPixelDown = pixelToWrite + m_FBWidth;
					if ( sbPixelDown >= 0 &&  // top clipping
							sbPixelDown < fbSize && // bottom clipping
							xTranslatedBack >= 0 && // left clipping
							xTranslatedBack < m_FBWidth && // right clipping
							! (color.m_IsMonochrome && color.m_M == 0.0f) )
					{
							m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, sbPixelDown );
					}
					*/

					nNCurrentY += 1.0f;
					pixelsMovedDown++;
					newY++;
				}

				nNCurrentX += 1.0f;
				xPixelsSkipped++;
				pixelsMovedRight++;
				currentXInt++;
			}

			nNCurrentX = std::fmod( nNCurrentX, nNXTravel );
		}

		nNYLeftOver = std::fmod( nNCurrentY, nNYTravel );
		nNCurrentX = 0.0f;
		currentXInt -= pixelsMovedRight;
		currentYInt += pixelsMovedDown;
	}
}

bool SoftwareGraphics::clipLine (float* xStart, float* yStart, float* xEnd, float* yEnd)
{
	// cache points
	float xStartClipped = *xStart;
	float yStartClipped = *yStart;
	float xEndClipped   = *xEnd;
	float yEndClipped   = *yEnd;

	// regions
	const unsigned int INSIDE = 0b0000;
	const unsigned int LEFT   = 0b0001;
	const unsigned int RIGHT  = 0b0010;
	const unsigned int BOTTOM = 0b0100;
	const unsigned int TOP    = 0b1000;

	// initialize point regions as being inside screen rect
	unsigned int point1Region = INSIDE;
	unsigned int point2Region = INSIDE;

	// determine regions of point 1
	if (xStartClipped < 0.0f)
	{
		point1Region |= LEFT;
	}
	else if (xStartClipped > 1.0f)
	{
		point1Region |= RIGHT;
	}
	if (yStartClipped < 0.0f)
	{
		point1Region |= TOP;
	}
	else if (yStartClipped > 1.0f)
	{
		point1Region |= BOTTOM;
	}

	// determine regions of point 2
	if (xEndClipped < 0.0f)
	{
		point2Region |= LEFT;
	}
	else if (xEndClipped > 1.0f)
	{
		point2Region |= RIGHT;
	}
	if (yEndClipped < 0.0f)
	{
		point2Region |= TOP;
	}
	else if (yEndClipped > 1.0f)
	{
		point2Region |= BOTTOM;
	}

	if ( (point1Region == INSIDE) && (point2Region == INSIDE) ) // if both points exist inside the screen rect
	{
		return true;
	}
	else if ( point1Region & point2Region ) // if both points exist outside the screen rect in the same region
	{
		return false;
	}
	else // if at least one point exists outside of the screen rect and points exist in different regions
	{
		float tempX1 = xStartClipped;
		float tempY1 = yStartClipped;
		float tempX2 = xEndClipped;
		float tempY2 = yEndClipped;

		if (point1Region != INSIDE)
		{
			// clip point1 based on region
			if (point1Region & TOP)
			{
				if (point1Region & RIGHT)
				{
					tempX1 = 1.0f;
					tempY1 = yStartClipped + (yEndClipped - yStartClipped) *
						(1.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY1 < 0.0f) // if the line is still outside, clip again
					{
						tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
							(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY1 = 0.0f;
					}
				}
				else if (point1Region & LEFT)
				{
					tempX1 = 0.0f;
					tempY1 = yStartClipped + (yEndClipped - yStartClipped) *
						(0.0f - xStartClipped) / (xEndClipped - xStartClipped);
					if (tempY1 < 0.0f) // if the line is still outside, clip again
					{
						tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
							(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY1 = 0.0f;
					}
				}
				else
				{
					tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
						(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
					tempY1 = 0.0f;
				}
			}
			else if (point1Region & BOTTOM)
			{
				if (point1Region & RIGHT)
				{
					tempX1 = 1.0f;
					tempY1 = yStartClipped + (yEndClipped - yStartClipped) *
						(1.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY1 > 1.0f) // if the line is still outside, clip again
					{
						tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
							(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY1 = 1.0f;
					}
				}
				else if (point1Region & LEFT)
				{
					tempX1 = 0.0f;
					tempY1 = yStartClipped + (yEndClipped - yStartClipped) *
						(0.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY1 > 1.0f) // if the line is still outside, clip again
					{
						tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
							(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY1 = 1.0f;
					}
				}
				else
				{
					tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
						(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
					tempY1 = 1.0f;
				}
			}
			else if (point1Region & RIGHT)
			{
				tempY1 = yStartClipped + (yEndClipped - yStartClipped) * (1.0f - xStartClipped) / (xEndClipped - xStartClipped);

				tempX1 = 1.0f;
			}
			else if (point1Region & LEFT)
			{
				tempY1 = yStartClipped + (yEndClipped - yStartClipped) * (0.0f - xStartClipped) / (xEndClipped - xStartClipped);

				tempX1 = 0.0f;
			}
		}

		if (point2Region != INSIDE)
		{
			// clip point2 based on region
			if (point2Region & TOP)
			{
				if (point2Region & RIGHT)
				{
					tempX2 = 1.0f;
					tempY2 = yStartClipped + (yEndClipped - yStartClipped) *
						(1.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY2 < 0.0f) // if the line is still outside, clip again
					{
						tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
							(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY2 = 0.0f;
					}
				}
				else if (point2Region & LEFT)
				{
					tempX2 = 0.0f;
					tempY2 = yStartClipped + (yEndClipped - yStartClipped) *
						(0.0f - xStartClipped) / (xEndClipped - xStartClipped);
					if (tempY2 < 0.0f) // if the line is still outside, clip again
					{
						tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
							(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY2 = 0.0f;
					}
				}
				else
				{
					tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
						(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
					tempY2 = 0.0f;
				}
			}
			else if (point2Region & BOTTOM)
			{
				if (point2Region & RIGHT)
				{
					tempX2 = 1.0f;
					tempY2 = yStartClipped + (yEndClipped - yStartClipped) *
						(1.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY2 > 1.0f) // if the line is still outside, clip again
					{
						tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
							(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY2 = 1.0f;
					}
				}
				else if (point2Region & LEFT)
				{
					tempX2 = 0.0f;
					tempY2 = yStartClipped + (yEndClipped - yStartClipped) *
						(0.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY2 > 1.0f) // if the line is still outside, clip again
					{
						tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
							(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY2 = 1.0f;
					}
				}
				else
				{
					tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
						(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
					tempY2 = 1.0f;
				}
			}
			else if (point2Region & RIGHT)
			{
				tempY2 = yStartClipped + (yEndClipped - yStartClipped) * (1.0f - xStartClipped) / (xEndClipped - xStartClipped);

				tempX2 = 1.0f;
			}
			else if (point2Region & LEFT)
			{
				tempY2 = yStartClipped + (yEndClipped - yStartClipped) * (0.0f - xStartClipped) / (xEndClipped - xStartClipped);

				tempX2 = 0.0f;
			}
		}

		// if x1 is still negative or over one, the entire line is off screen
		if (tempX1 < 0.0f || tempX1 > 1.0f) return false;

		// if y1 is still negative or over one, the entire line is off screen
		if (tempY1 < 0.0f || tempX1 > 1.0f) return false;

		xStartClipped = tempX1;
		yStartClipped = tempY1;
		xEndClipped   = tempX2;
		yEndClipped   = tempY2;
	}

	// replace original points with clipped points
	*xStart = xStartClipped;
	*yStart = yStartClipped;
	*xEnd   = xEndClipped;
	*yEnd   = yEndClipped;

	return true;
}


