#ifndef SOFTWAREGRAPHICS_HPP
#define SOFTWAREGRAPHICS_HPP

/**************************************************************************
 * The SofwareGraphics class defines functions used to render graphics
 * with CPU-based rendering, as opposed to GPU-based rendering. This is
 * useful for embedded applications where a GPU may not be available. The
 * constructor is protected, so that only a Surface object can construct
 * an instance and all drawing code can take place in the context of the
 * surface.
**************************************************************************/

#include "Graphics.hpp"

#define m_CP Graphics<width, height, format, bufferSize>::m_ColorProfile
#define m_CurrentFont Graphics<width, height, format, bufferSize>::m_CurrentFont
#define m_Pxls Graphics<width, height, format, bufferSize>::m_FB.getPixels()
#define m_NumPxls Graphics<width, height, format, bufferSize>::m_FB.getNumPixels()

#include "Font.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"
#include "Engine3D.hpp"

#include <algorithm>
#include <limits>

#include <iostream>

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
class SoftwareGraphics 	: public Graphics<width, height, format, bufferSize>
{
	// only a surface should be able to construct
	template<unsigned int w, unsigned int h, CP_FORMAT f, unsigned int bS> friend class SurfaceThreaded;
	template<unsigned int w, unsigned int h, CP_FORMAT f, unsigned int bS> friend class SurfaceSingleCore;

	public:
		void setColor (float r, float g, float b) override;
		void setColor (bool val) override;
		void setFont (Font* font) override;

		void fill() override;
		void drawLine (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawBox (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3) override;
		void drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3) override;
		// void drawTriangleShaded (Face& face, const Camera3D& camera) override;
		void drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
		void drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
		void drawCircle (float originX, float originY, float radius) override;
		void drawCircleFilled (float originX, float originY, float radius) override;
		void drawText (float xStart, float yStart, const char* text, float scaleFactor) override;
		// void drawSprite (float xStart, float yStart, Sprite& sprite) override;

		// TODO remove this after testing
		// void testTexture (Texture& texture) override;

	protected:
		void drawCircleHelper (int originX, int originY, int x, int y, bool filled = false);

		SoftwareGraphics();
		~SoftwareGraphics() override;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
SoftwareGraphics<width, height, format, bufferSize>::SoftwareGraphics() :
	Graphics<width, height, format, bufferSize>()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
SoftwareGraphics<width, height, format, bufferSize>::~SoftwareGraphics()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::setColor (float r, float g, float b)
{
	m_CP.setColor( r, g, b );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::setColor (bool val)
{
	m_CP.setColor( val );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::setFont (Font* font)
{
	m_CurrentFont = font;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::fill()
{
	for ( unsigned int pixelNum = 0; pixelNum < width * height; pixelNum++ )
	{
		m_CP.template putPixel<width, height>( m_Pxls, pixelNum );
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawLine (float xStart, float yStart, float xEnd, float yEnd)
{
	// clip line and return if line is off screen
	if ( !Graphics<width, height, format, bufferSize>::clipLine( &xStart, &yStart, &xEnd, &yEnd ) ) return;

	unsigned int xStartUInt = xStart * (width  - 1);
	unsigned int yStartUInt = yStart * (height - 1);
	unsigned int xEndUInt   = xEnd   * (width  - 1);
	unsigned int yEndUInt   = yEnd   * (height - 1);

	float slope = ((float) yEndUInt - yStartUInt) / ((float)xEndUInt - xStartUInt);

	unsigned int pixelStart = ( (width * yStartUInt) + xStartUInt );
	unsigned int pixelEnd   = ( (width * yEndUInt  ) + xEndUInt   );

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
				m_CP.template putPixel<width, height>( m_Pxls, pixel );
			}
			else if (slope < 1.0f) // stride along x-axis across multiple pixels
			{
				while (xAccumulator < 1.0f && pixel <= pixelEnd)
				{
					m_CP.template putPixel<width, height>( m_Pxls, pixel );

					xAccumulator += slope;
					pixel += 1;
				}
				xAccumulator = xAccumulator - 1.0f;
			}
			else // stride along x-axis by one pixel
			{
				m_CP.template putPixel<width, height>( m_Pxls, pixel );

				pixel += 1;
			}

			// y stride
			if (slope >= 1.0f) // stride along the y-axis across multiple pixels
			{
				while (yAccumulator >= 1.0f && pixel <= pixelEnd)
				{
					m_CP.template putPixel<width, height>( m_Pxls, pixel );

					yAccumulator -= 1.0f;
					pixel += width;
				}
				yAccumulator += slope;
			}
			else // stride along the y-axis by one pixel
			{
				pixel += width;
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
				m_CP.template putPixel<width, height>( m_Pxls, pixel );
			}
			else if (slope > -1.0f) // stride along x-axis across multiple pixels
			{
				while (xAccumulator > -1.0f && pixel <= pixelEnd)
				{
					m_CP.template putPixel<width, height>( m_Pxls, pixel );

					xAccumulator += slope;
					pixel -= 1;
				}
				xAccumulator = xAccumulator + 1.0f;
			}
			else // stride along x-axis by one pixel
			{
				m_CP.template putPixel<width, height>( m_Pxls, pixel );

				pixel -= 1;
			}

			// y stride
			if (slope <= -1.0f) // stride along the y-axis across multiple pixels
			{
				while (yAccumulator <= 1.0f && pixel <= pixelEnd)
				{
					m_CP.template putPixel<width, height>( m_Pxls, pixel );

					yAccumulator += 1.0f;
					pixel += width;
				}
				yAccumulator += slope;
			}
			else // stride along the y-axis by one pixel
			{
				pixel += width;

				m_CP.template putPixel<width, height>( m_Pxls, pixel );
			}
		}
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawBox (float xStart, float yStart, float xEnd, float yEnd)
{
	drawLine( xStart, yStart, xEnd,   yStart );
	drawLine( xEnd,   yStart, xEnd,   yEnd   );
	drawLine( xEnd,   yEnd,   xStart, yEnd   );
	drawLine( xStart, yEnd,   xStart, yStart );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd)
{
	int xStartUInt = xStart * (width  - 1);
	int yStartUInt = yStart * (height - 1);
	int xEndUInt   = xEnd   * (width  - 1);
	int yEndUInt   = yEnd   * (height - 1);

	// clipping
	xStartUInt = std::min( std::max(xStartUInt, 0), (int)width );
	yStartUInt = std::min( std::max(yStartUInt, 0), (int)height );
	xEndUInt   = std::min( std::max(xEndUInt,   0), (int)width );
	yEndUInt   = std::min( std::max(yEndUInt,   0), (int)height );

	const unsigned int pixelRowStride = abs( (int)xEndUInt - (int)xStartUInt );

	unsigned int pixelStart = ( (width * yStartUInt) + xStartUInt );
	unsigned int pixelEnd   = ( (width * yEndUInt  ) + xEndUInt   );

	if (pixelStart > pixelEnd)
	{
		// swap
		unsigned int temp = pixelStart;
		pixelStart = pixelEnd;
		pixelEnd   = temp;
	}

	const unsigned int pStart = pixelStart;
	const unsigned int pEnd = pixelEnd;

	for (unsigned int pixel = pStart; pixel < pEnd; pixel += width)
	{
		const unsigned int pixelRowEnd = pixel + pixelRowStride;

		for (unsigned int rowPixel = pixel; rowPixel < pixelRowEnd; rowPixel += 1)
		{
			m_CP.template putPixel<width, height>( m_Pxls, rowPixel );
		}
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3)
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

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3)
{
	// getting the pixel values of the vertices
	int x1UInt = x1 * (width  - 1);
	int y1UInt = y1 * (height - 1);
	int x2UInt = x2 * (width  - 1);
	int y2UInt = y2 * (height - 1);
	int x3UInt = x3 * (width  - 1);
	int y3UInt = y3 * (height - 1);

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
		xRightAccumulator = (float) x2Sorted;

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
			xRightAccumulator = (float) x3Sorted;
		}

		// if after clipping this line exists within the screen, render the line
		if ( Graphics<width, height, format, bufferSize>::clipLine(&tempX1, &tempY1, &tempX2, &tempY2) )
		{
			int tempX1Int = tempX1 * (width  - 1);
			int tempY1Int = tempY1 * (height - 1);
			int tempX2Int = tempX2 * (width  - 1);
			int tempY2Int = tempY2 * (height - 1);

			const int tempXY1 = ( (tempY1Int * width) + tempX1Int );
			const int tempXY2 = ( (tempY2Int * width) + tempX2Int );

			for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
			{
				m_CP.template putPixel<width, height>( m_Pxls, pixel );
			}
		}
	}
	else
	{
		// render up until the second vertice
		for (int row = y1Sorted; row <= y2Sorted; row++)
		{
			// clip vertically if row is off screen
			if (row >= (int)height)
			{
				break;
			}
			else if (row >= 0)
			{
				// rounding the points and clipping horizontally
				unsigned int leftX  = std::min( std::max((int)std::round(xLeftAccumulator), 0), (int)width - 1 );
				unsigned int rightX = std::max( std::min((int)std::round(xRightAccumulator), (int)width - 1), 0 );

				unsigned int tempXY1 = ( (row * width) + leftX  );
				unsigned int tempXY2 = ( (row * width) + rightX );

				for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
				{
					m_CP.template putPixel<width, height>( m_Pxls, pixel );
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

	// rasterize up until the last vertice
	if (y2Sorted != y3Sorted) // if the bottom of the triangle isn't a horizontal line
	{
		for (int row = y2Sorted + 1; row <= y3Sorted; row++)
		{
			// clip vertically if row is off screen
			if (row >= (int)height)
			{
				break;
			}
			else if ( row >= 0 )
			{
				// rounding the points and clipping horizontally
				unsigned int leftX  = std::min( std::max((int)std::round(xLeftAccumulator), 0), (int)width - 1 );
				unsigned int rightX = std::max( std::min((int)std::round(xRightAccumulator), (int)width - 1), 0 );

				unsigned int tempXY1 = ( (row * width) + leftX  );
				unsigned int tempXY2 = ( (row * width) + rightX );

				for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
				{
					m_CP.template putPixel<width, height>( m_Pxls, pixel );
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

static inline void calcTriGradients( float& v1StartEdgeDistT, float& v1EndEdgeDistT, float& v1StartEdgeDistB, float& v1EndEdgeDistB,
					float& v2StartEdgeDistT, float& v2EndEdgeDistT, float& v2StartEdgeDistB, float& v2EndEdgeDistB,
					float& v3StartEdgeDistT, float &v3EndEdgeDistT, float& v3StartEdgeDistB, float& v3EndEdgeDistB,
					const float& xy1DistToXy2, const float& xy1DistToXy3, const float& xy2DistToXy3, const int& y1UInt,
					const int& y1Sorted, const int& y2Sorted, const int &y3Sorted, const int& x1Int, const int& x2Int,
					const int& x3Int, const int& y2Int, const int& y3Int, const bool& needsSwapping, const float& zeroVal )
{
	if ( y1UInt == y1Sorted &&
		(y1Sorted < y2Sorted || (y1Sorted == y2Sorted && x1Int < x2Int)) ) // xy1 is on top
	{
		// first assuming that xy2 is above and to the left of xy3
		v1StartEdgeDistT = xy1DistToXy2;
		v1EndEdgeDistT = xy1DistToXy3;
		v2StartEdgeDistT = xy1DistToXy2;
		v3EndEdgeDistT = xy1DistToXy3;
		v1StartEdgeDistB = xy1DistToXy2;
		v1EndEdgeDistB = xy1DistToXy3;
		v2StartEdgeDistB = xy2DistToXy3;
		v3StartEdgeDistB = xy2DistToXy3;
		v3EndEdgeDistB = xy1DistToXy3;

		if ( x3Int < x2Int )
		{
			v1StartEdgeDistB = zeroVal;
			v1EndEdgeDistB = xy1DistToXy3;

			if ( y3Int < y2Int )
			{
				if ( needsSwapping )
				{
					v1StartEdgeDistB = xy1DistToXy2;
					v1EndEdgeDistB = zeroVal;
					v2StartEdgeDistB = xy1DistToXy2;
					v2EndEdgeDistB = xy2DistToXy3;
					v3StartEdgeDistB = zeroVal;
					v3EndEdgeDistB = xy2DistToXy3;
				}
				else
				{
					v1StartEdgeDistT = xy1DistToXy3;
					v1EndEdgeDistT = xy1DistToXy2;
					v1StartEdgeDistB = zeroVal;
					v1EndEdgeDistB = xy1DistToXy2;
					v2StartEdgeDistT = zeroVal;
					v2EndEdgeDistT = xy1DistToXy2;
					v2EndEdgeDistB = xy1DistToXy2;
					v3StartEdgeDistT = xy1DistToXy3;
					v3EndEdgeDistT = zeroVal;
					v3StartEdgeDistB = xy2DistToXy3;
					v3EndEdgeDistB = zeroVal;
				}
			}
			else if ( needsSwapping )
			{
				v1StartEdgeDistT = xy1DistToXy3;
				v1EndEdgeDistT = xy1DistToXy2;
				v1StartEdgeDistB = xy1DistToXy3;
				v1EndEdgeDistB = zeroVal;
				v2StartEdgeDistT = zeroVal;
				v2EndEdgeDistT = xy1DistToXy2;
				v2StartEdgeDistB = zeroVal;
				v2EndEdgeDistB = xy2DistToXy3;
				v3StartEdgeDistT = xy1DistToXy3;
				v3EndEdgeDistT = zeroVal;
				v3StartEdgeDistB = xy1DistToXy3;
				v3EndEdgeDistB = xy2DistToXy3;
			}
		}
		else if ( y3Int < y2Int )
		{
			if ( needsSwapping )
			{
				v2StartEdgeDistB = xy1DistToXy2;
				v2EndEdgeDistB = xy2DistToXy3;
				v3StartEdgeDistB = zeroVal;
				v3EndEdgeDistB = xy2DistToXy3;
			}
			else
			{
				v1StartEdgeDistT = xy1DistToXy3;
				v1EndEdgeDistT = xy1DistToXy2;
				v1StartEdgeDistB = zeroVal;
				v1EndEdgeDistB = xy1DistToXy2;
				v2StartEdgeDistT = zeroVal;
				v2EndEdgeDistT = xy1DistToXy2;
				v2StartEdgeDistB = xy2DistToXy3;
				v2EndEdgeDistB = xy1DistToXy2;
				v3StartEdgeDistT = xy1DistToXy3;
				v3EndEdgeDistT = zeroVal;
				v3StartEdgeDistB = xy2DistToXy3;
				v3EndEdgeDistB = zeroVal;
			}
		}
		else if ( needsSwapping )
		{
			v1StartEdgeDistT = xy1DistToXy3;
			v1EndEdgeDistT = xy1DistToXy2;
			v1StartEdgeDistB = xy1DistToXy3;
			v1EndEdgeDistB = zeroVal;
			v2StartEdgeDistT = zeroVal;
			v2EndEdgeDistT = xy1DistToXy2;
			v2StartEdgeDistB = zeroVal;
			v2EndEdgeDistB = xy2DistToXy3;
			v3StartEdgeDistT = xy1DistToXy3;
			v3EndEdgeDistT = zeroVal;
			v3StartEdgeDistB = xy1DistToXy3;
			v3EndEdgeDistB = xy2DistToXy3;
		}
	}
	else if ( y1UInt == y2Sorted &&
		(y2Sorted < y3Sorted || (y2Sorted == y3Sorted && x1Int < x2Int)) ) // xy1 is in the middle
	{
		// first assuming that xy2 is above and to the left of xy3
		v1StartEdgeDistT = xy1DistToXy2;
		v2StartEdgeDistT = xy1DistToXy2;
		v2EndEdgeDistT = xy2DistToXy3;
		v3EndEdgeDistT = xy2DistToXy3;
		v1StartEdgeDistB = xy1DistToXy3;
		v2EndEdgeDistB = xy2DistToXy3;
		v3StartEdgeDistB = xy1DistToXy3;
		v3EndEdgeDistB = xy2DistToXy3;

		if ( x3Int < x2Int )
		{
			if ( y3Int < y2Int )
			{
				if ( needsSwapping )
				{
					v1StartEdgeDistT = zeroVal;
					v1EndEdgeDistT = xy1DistToXy3;
					v1StartEdgeDistB = zeroVal;
					v1EndEdgeDistB = xy1DistToXy2;
					v2StartEdgeDistT = xy2DistToXy3;
					v2EndEdgeDistT = zeroVal;
					v2StartEdgeDistB = xy2DistToXy3;
					v2EndEdgeDistB = xy1DistToXy2;
					v3StartEdgeDistT = xy2DistToXy3;
					v3EndEdgeDistT = xy1DistToXy3;
					v3StartEdgeDistB = xy2DistToXy3;
					v3EndEdgeDistB = zeroVal;
				}
				else
				{
					v1StartEdgeDistT = xy1DistToXy3;
					v1StartEdgeDistB = xy1DistToXy2;
					v2StartEdgeDistT = zeroVal;
					v2StartEdgeDistB = xy1DistToXy2;
					v3StartEdgeDistT = xy1DistToXy3;
					v3StartEdgeDistB = zeroVal;
					v3EndEdgeDistB = xy2DistToXy3;
				}
			}
			else if ( needsSwapping )
			{
				v1StartEdgeDistT = zeroVal;
				v1EndEdgeDistT = xy1DistToXy2;
				v1StartEdgeDistB = zeroVal;
				v1EndEdgeDistB = xy1DistToXy3;
				v2StartEdgeDistT = xy2DistToXy3;
				v2EndEdgeDistT = xy1DistToXy2;
				v2StartEdgeDistB = xy2DistToXy3;
				v2EndEdgeDistB = zeroVal;
				v3StartEdgeDistT = xy2DistToXy3;
				v3EndEdgeDistT = zeroVal;
				v3StartEdgeDistB = xy2DistToXy3;
				v3EndEdgeDistB = xy1DistToXy3;
			}
		}
		else if ( y3Int < y2Int )
		{
			if ( needsSwapping )
			{
				v1StartEdgeDistT = zeroVal;
				v1EndEdgeDistT = xy1DistToXy3;
				v1StartEdgeDistB = zeroVal;
				v1EndEdgeDistB = xy1DistToXy2;
				v2StartEdgeDistT = xy2DistToXy3;
				v2EndEdgeDistT = zeroVal;
				v2StartEdgeDistB = xy2DistToXy3;
				v2EndEdgeDistB = xy1DistToXy2;
				v3StartEdgeDistT = xy2DistToXy3;
				v3EndEdgeDistT = xy1DistToXy3;
				v3StartEdgeDistB = xy2DistToXy3;
				v3EndEdgeDistB = zeroVal;
			}
			else
			{
				v1StartEdgeDistT = xy1DistToXy3;
				v1StartEdgeDistB = xy1DistToXy2;
				v2StartEdgeDistT = zeroVal;
				v2StartEdgeDistB = xy1DistToXy2;
				v3StartEdgeDistT = xy1DistToXy3;
				v3StartEdgeDistB = zeroVal;
			}
		}
		else if ( needsSwapping )
		{
			v1StartEdgeDistT = zeroVal;
			v1EndEdgeDistT = xy1DistToXy2;
			v2StartEdgeDistT = xy2DistToXy3;
			v2EndEdgeDistT = xy1DistToXy2;
			v3StartEdgeDistT = xy2DistToXy3;
			v3EndEdgeDistT = zeroVal;
			v1StartEdgeDistB = zeroVal;
			v1EndEdgeDistB = xy1DistToXy3;
			v2StartEdgeDistB = xy2DistToXy3;
			v2EndEdgeDistB = zeroVal;
			v3StartEdgeDistB = xy2DistToXy3;
			v3EndEdgeDistB = xy1DistToXy3;
		}
	}
	else if ( y1UInt == y3Sorted ) // xy1 is on the bottom
	{
		// first assuming that xy2 is above and to the left of xy3
		v1EndEdgeDistT = xy1DistToXy2;
		v1StartEdgeDistB = xy1DistToXy3;
		v1EndEdgeDistB = xy1DistToXy2;
		v2StartEdgeDistT = xy2DistToXy3;
		v2EndEdgeDistT = xy1DistToXy2;
		v2EndEdgeDistB = xy1DistToXy2;
		v3StartEdgeDistT = xy2DistToXy3;
		v3StartEdgeDistB = xy1DistToXy3;

		if ( x3Int < x2Int )
		{
			if ( y3Int <= y2Int )
			{
				if ( needsSwapping )
				{
					v1StartEdgeDistT = xy1DistToXy3;
					v1EndEdgeDistT = zeroVal;
					v2StartEdgeDistT = zeroVal;
					v2EndEdgeDistT = xy2DistToXy3;
					v2StartEdgeDistB = zeroVal;
					v3StartEdgeDistT = xy1DistToXy3;
					v3EndEdgeDistT = xy2DistToXy3;
					v3EndEdgeDistB = zeroVal;
				}
				else
				{
					v1EndEdgeDistT = xy1DistToXy3;
					v1StartEdgeDistB = xy1DistToXy2;
					v1EndEdgeDistB = xy1DistToXy3;
					v2EndEdgeDistT = zeroVal;
					v2StartEdgeDistB = xy1DistToXy2;
					v2EndEdgeDistB = zeroVal;
					v3EndEdgeDistT = xy1DistToXy3;
					v3StartEdgeDistB = zeroVal;
					v3EndEdgeDistB = xy1DistToXy3;
				}
			}
			else if ( needsSwapping )
			{
				v1StartEdgeDistT = xy1DistToXy2;
				v1EndEdgeDistT = zeroVal;
				v1StartEdgeDistB = xy1DistToXy2;
				v1EndEdgeDistB = xy1DistToXy3;
				v2StartEdgeDistT = xy1DistToXy2;
				v2EndEdgeDistT = xy2DistToXy3;
				v2StartEdgeDistB = xy1DistToXy2;
				v2EndEdgeDistB = zeroVal;
				v3StartEdgeDistT = zeroVal;
				v3EndEdgeDistT = xy2DistToXy3;
				v3StartEdgeDistB = zeroVal;
				v3EndEdgeDistB = xy1DistToXy3;
			}
		}
		else if ( y3Int < y2Int )
		{
			if ( needsSwapping )
			{
				v1StartEdgeDistT = xy1DistToXy3;
				v1EndEdgeDistT = zeroVal;
				v2StartEdgeDistT = zeroVal;
				v2EndEdgeDistT = xy2DistToXy3;
				v3StartEdgeDistT = xy1DistToXy3;
				v3EndEdgeDistT = xy2DistToXy3;
				v3StartEdgeDistB = xy1DistToXy3;
				v3EndEdgeDistB = zeroVal;
			}
			else
			{
				v2EndEdgeDistT = zeroVal;
				v2StartEdgeDistB = xy1DistToXy2;
				v2EndEdgeDistB = zeroVal;
				v3EndEdgeDistT = xy1DistToXy3;
				v3StartEdgeDistB = zeroVal;
				v3EndEdgeDistB = xy1DistToXy3;
			}
		}
		else if ( needsSwapping )
		{
			v1StartEdgeDistT = xy1DistToXy2;
			v1EndEdgeDistT = zeroVal;
			v1StartEdgeDistB = xy1DistToXy2;
			v1EndEdgeDistB = xy1DistToXy3;
			v2StartEdgeDistT = xy1DistToXy2;
			v2EndEdgeDistT = xy2DistToXy3;
			v2StartEdgeDistB = xy1DistToXy2;
			v2EndEdgeDistB = zeroVal;
			v3StartEdgeDistT = zeroVal;
			v3EndEdgeDistT = xy2DistToXy3;
			v3StartEdgeDistB = zeroVal;
			v3EndEdgeDistB = xy1DistToXy3;
		}
	}
}

/*
void SoftwareGraphics::drawTriangleShaded (Face& face, const Camera3D& camera)
{
	// get previous color, since we'll want to set it back when we're done with the shading colors
	const Color previousColor = m_CP->getColor();

	// a color to store for fragment shading
	Color currentColor = m_CP->getColor();

	// put through the vertex shader first
	this->vertexShader( face );

	// first determine if this triangle needs to be rendered (back face culling)
	face.calcNormals();
	const Vector<4>& vertexVec = face.vertices[0].vec;
	const Vector<4>& normal = face.normal;
	if ( normal.x() * (vertexVec.x() - camera.x())
			+ normal.y() * (vertexVec.y() - camera.y())
			+ normal.z() * (vertexVec.z() - camera.z()) < 0.0f )
	{
		camera.projectFace( face );
		camera.scaleXYToZeroToOne( face );
	}
	else // normal not facing camera, so no need to render
	{
		return;
	}

	float x1 = face.vertices[0].vec.x();
	float y1 = face.vertices[0].vec.y();
	float x2 = face.vertices[1].vec.x();
	float y2 = face.vertices[1].vec.y();
	float x3 = face.vertices[2].vec.x();
	float y3 = face.vertices[2].vec.y();

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
	float texCoordX1 = face.vertices[0].texCoords.x();
	float texCoordY1 = face.vertices[0].texCoords.y();
	float texCoordX2 = face.vertices[1].texCoords.x();
	float texCoordY2 = face.vertices[1].texCoords.y();
	float texCoordX3 = face.vertices[2].texCoords.x();
	float texCoordY3 = face.vertices[2].texCoords.y();

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
	float v1StartEdgeDistT = zeroVal; // T for rendering the top half of the triangle
	float v2StartEdgeDistT = zeroVal;
	float v3StartEdgeDistT = zeroVal;
	float v1EndEdgeDistT = zeroVal;
	float v2EndEdgeDistT = zeroVal;
	float v3EndEdgeDistT = zeroVal;
	float v1StartEdgeDistB = zeroVal; // B for rendering the bottom half of the triangle
	float v2StartEdgeDistB = zeroVal;
	float v3StartEdgeDistB = zeroVal;
	float v1EndEdgeDistB = zeroVal;
	float v2EndEdgeDistB = zeroVal;
	float v3EndEdgeDistB = zeroVal;
	float v1PerspMul = 1.0f / face.vertices[0].vec.w();
	float v2PerspMul = 1.0f / face.vertices[1].vec.w();
	float v3PerspMul = 1.0f / face.vertices[2].vec.w();
	// swap start and end values based on vertex positions
	calcTriGradients( v1StartEdgeDistT, v1EndEdgeDistT, v1StartEdgeDistB, v1EndEdgeDistB, v2StartEdgeDistT, v2EndEdgeDistT,
						v2StartEdgeDistB, v2EndEdgeDistB, v3StartEdgeDistT, v3EndEdgeDistT, v3StartEdgeDistB, v3EndEdgeDistB,
						xy1DistToXy2, xy1DistToXy3, xy2DistToXy3, y1Int, y1Sorted, y2Sorted, y3Sorted, x1Int, x2Int,
						x3Int, y2Int, y3Int, needsSwapping, zeroVal );

	// setting the y value for gradients
	float yInRelationToY1 = -1.0f * (1.0f - ((y3Sorted - y1Int) * yInRelationIncr));
	yInRelationToY1 = ( isnan(yInRelationToY1) ) ? 0.0f : yInRelationToY1;

	// if slope is zero, the top of the triangle is a horizontal line so fill the row to x2, y2 and skip for loop
	if ( line1Slope == 0.0f || isnan(line1Slope) )
	{
		xRightAccumulator = (float) x2Sorted;

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
			xRightAccumulator = (float) x3Sorted;
		}

		// if after clipping this line exists within the screen, render the line
		if ( clipLine(&tempX1, &tempY1, &tempX2, &tempY2) )
		{
			int unclippedLeftX = x1FSorted * ( m_FBWidth - 1 );
			int tempX1Int = tempX1 * ( m_FBWidth  - 1 );
			int tempY1Int = tempY1 * ( m_FBHeight - 1 );
			int tempX2Int = tempX2 * ( m_FBWidth  - 1 );
			int tempY2Int = tempY2 * ( m_FBHeight - 1 );

			int tempXY1 = ( (tempY1Int * m_FBWidth) + tempX1Int );
			int tempXY2 = ( (tempY2Int * m_FBWidth) + tempX2Int );

			// setting the x values for gradients
			float xLeftInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - xLeftAccumulator)
							* xInRelationIncr);
			float xRightInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - xRightAccumulator)
							* xInRelationIncr);
			// get important distances
			float xy1DistToXLeft = this->distance(0.0f, 0.0f, xLeftInRelationToX1, yInRelationToY1);
			float xy1DistToXRight = this->distance(0.0f, 0.0f, xRightInRelationToX1, yInRelationToY1);
			float xy2DistToXLeft = this->distance(x2InRelationToX1, y2InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
			float xy2DistToXRight = this->distance(x2InRelationToX1, y2InRelationToY1, xRightInRelationToX1, yInRelationToY1);
			float xy3DistToXLeft = this->distance(x3InRelationToX1, y3InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
			float xy3DistToXRight = this->distance(x3InRelationToX1, y3InRelationToY1, xRightInRelationToX1, yInRelationToY1);
			// compute the starting and ending color values for each scanline
			float v1Start = triGradNormalizedDist(xy1DistToXLeft, v1StartEdgeDistT);
			float v2Start = triGradNormalizedDist(xy2DistToXLeft, v2StartEdgeDistT);
			float v3Start = triGradNormalizedDist(xy3DistToXLeft, v3StartEdgeDistT);
			float v1End = triGradNormalizedDist(xy1DistToXRight, v1EndEdgeDistT);
			float v2End = triGradNormalizedDist(xy2DistToXRight, v2EndEdgeDistT);
			float v3End = triGradNormalizedDist(xy3DistToXRight, v3EndEdgeDistT);
			// linearly interpolate between the two values
			float v1Incr = (v1End - v1Start) / (xRightAccumulator - xLeftAccumulator);
			float v1Current = v1Start + ( v1Incr * std::abs(std::min(unclippedLeftX, 0)) );
			float v2Incr = (v2End - v2Start) / (xRightAccumulator - xLeftAccumulator);
			float v2Current = v2Start + ( v2Incr * std::abs(std::min(unclippedLeftX, 0)) );
			float v3Incr = (v3End - v3Start) / (xRightAccumulator - xLeftAccumulator);
			float v3Current = v3Start + ( v3Incr * std::abs(std::min(unclippedLeftX, 0)) );

			for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
			{
				float perspInterp = ( v1Current * v1PerspMul ) + ( v2Current * v2PerspMul ) + ( v3Current * v3PerspMul );
				perspInterp += 1.0f - ( v1Current + v2Current + v3Current ); // offset to protect from warping (hacky)
				perspInterp = 1.0f / perspInterp;
				float v1CurPersp = v1Current * ( v1PerspMul * perspInterp );
				float v2CurPersp = v2Current * ( v2PerspMul * perspInterp );
				float v3CurPersp = v3Current * ( v3PerspMul * perspInterp );
				float texCoordX = ( v1CurPersp * texCoordX1 ) + ( v2CurPersp * texCoordX2 ) + ( v3CurPersp * texCoordX3 );
				float texCoordY = ( v1CurPersp * texCoordY1 ) + ( v2CurPersp * texCoordY2 ) + ( v3CurPersp * texCoordY3 );
				this->fragmentShader( currentColor, face, m_CurrentTexture, v1CurPersp, v2CurPersp, v3CurPersp, texCoordX,
							texCoordY );
				m_CP->setColor( currentColor );
				m_CP->putPixel( m_FBPixels, m_FBNumPixels, pixel );

				v1Current += v1Incr;
				v2Current += v2Incr;
				v3Current += v3Incr;
			}
		}

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
				int unclippedLeftX = std::round( xLeftAccumulator );
				int unclippedRightX = std::round( xRightAccumulator );
				unsigned int leftX  = std::min( std::max((int)unclippedLeftX, 0), (int)m_FBWidth - 1 );
				unsigned int rightX = std::max( std::min((int)unclippedRightX, (int)m_FBWidth - 1), 0 );

				unsigned int tempXY1 = ( (row * m_FBWidth) + leftX  );
				unsigned int tempXY2 = ( (row * m_FBWidth) + rightX );

				// setting the x values for gradients
				float xLeftInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - xLeftAccumulator)
								* xInRelationIncr);
				float xRightInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - xRightAccumulator)
								* xInRelationIncr);
				// get important distances
				float xy1DistToXLeft = this->distance(0.0f, 0.0f, xLeftInRelationToX1, yInRelationToY1);
				float xy1DistToXRight = this->distance(0.0f, 0.0f, xRightInRelationToX1, yInRelationToY1);
				float xy2DistToXLeft = this->distance(x2InRelationToX1, y2InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
				float xy2DistToXRight = this->distance(x2InRelationToX1, y2InRelationToY1, xRightInRelationToX1, yInRelationToY1);
				float xy3DistToXLeft = this->distance(x3InRelationToX1, y3InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
				float xy3DistToXRight = this->distance(x3InRelationToX1, y3InRelationToY1, xRightInRelationToX1, yInRelationToY1);
				// compute the starting and ending color values for each scanline
				float v1Start = triGradNormalizedDist(xy1DistToXLeft, v1StartEdgeDistT);
				float v2Start = triGradNormalizedDist(xy2DistToXLeft, v2StartEdgeDistT);
				float v3Start = triGradNormalizedDist(xy3DistToXLeft, v3StartEdgeDistT);
				float v1End = triGradNormalizedDist(xy1DistToXRight, v1EndEdgeDistT);
				float v2End = triGradNormalizedDist(xy2DistToXRight, v2EndEdgeDistT);
				float v3End = triGradNormalizedDist(xy3DistToXRight, v3EndEdgeDistT);
				// linearly interpolate between the two values
				float v1Incr = (v1End - v1Start) / (xRightAccumulator - xLeftAccumulator);
				float v1Current = v1Start + ( v1Incr * std::abs(std::min(unclippedLeftX, 0)) );
				float v2Incr = (v2End - v2Start) / (xRightAccumulator - xLeftAccumulator);
				float v2Current = v2Start + ( v2Incr * std::abs(std::min(unclippedLeftX, 0)) );
				float v3Incr = (v3End - v3Start) / (xRightAccumulator - xLeftAccumulator);
				float v3Current = v3Start + ( v3Incr * std::abs(std::min(unclippedLeftX, 0)) );

				for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
				{
					float perspInterp = ( v1Current * v1PerspMul ) + ( v2Current * v2PerspMul ) + ( v3Current * v3PerspMul );
					perspInterp += 1.0f - ( v1Current + v2Current + v3Current ); // offset to protect from warping (hacky)
					perspInterp = 1.0f / perspInterp;
					float v1CurPersp = v1Current * ( v1PerspMul * perspInterp );
					float v2CurPersp = v2Current * ( v2PerspMul * perspInterp );
					float v3CurPersp = v3Current * ( v3PerspMul * perspInterp );
					float texCoordX = ( v1CurPersp * texCoordX1 ) + ( v2CurPersp * texCoordX2 ) + ( v3CurPersp * texCoordX3 );
					float texCoordY = ( v1CurPersp * texCoordY1 ) + ( v2CurPersp * texCoordY2 ) + ( v3CurPersp * texCoordY3 );
					this->fragmentShader( currentColor, face, m_CurrentTexture, v1CurPersp, v2CurPersp, v3CurPersp, texCoordX,
								texCoordY );
					m_CP->setColor( currentColor );
					m_CP->putPixel( m_FBPixels, m_FBNumPixels, pixel );

					v1Current += v1Incr;
					v2Current += v2Incr;
					v3Current += v3Incr;
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
				int unclippedLeftX = std::round( xLeftAccumulator );
				int unclippedRightX = std::round( xRightAccumulator );
				unsigned int leftX  = std::min( std::max((int)unclippedLeftX, 0), (int)m_FBWidth - 1 );
				unsigned int rightX = std::max( std::min((int)unclippedRightX, (int)m_FBWidth - 1), 0 );

				unsigned int tempXY1 = ( (row * m_FBWidth) + leftX  );
				unsigned int tempXY2 = ( (row * m_FBWidth) + rightX );

				// setting the x values for gradients
				float xLeftInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - xLeftAccumulator) * xInRelationIncr);
				float xRightInRelationToX1 = (1.0f - x1InRelationToXLeft) - ((xInRelationRightmost - xRightAccumulator) * xInRelationIncr);
				// get important distances
				float xy1DistToXLeft = this->distance(0.0f, 0.0f, xLeftInRelationToX1, yInRelationToY1);
				float xy1DistToXRight = this->distance(0.0f, 0.0f, xRightInRelationToX1, yInRelationToY1);
				float xy2DistToXLeft = this->distance(x2InRelationToX1, y2InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
				float xy2DistToXRight = this->distance(x2InRelationToX1, y2InRelationToY1, xRightInRelationToX1, yInRelationToY1);
				float xy3DistToXLeft = this->distance(x3InRelationToX1, y3InRelationToY1, xLeftInRelationToX1, yInRelationToY1);
				float xy3DistToXRight = this->distance(x3InRelationToX1, y3InRelationToY1, xRightInRelationToX1, yInRelationToY1);
				// compute the starting and ending color values for each scanline
				float v1Start = triGradNormalizedDist(xy1DistToXLeft, v1StartEdgeDistB);
				float v2Start = triGradNormalizedDist(xy2DistToXLeft, v2StartEdgeDistB);
				float v3Start = triGradNormalizedDist(xy3DistToXLeft, v3StartEdgeDistB);
				float v1End = triGradNormalizedDist(xy1DistToXRight, v1EndEdgeDistB);
				float v2End = triGradNormalizedDist(xy2DistToXRight, v2EndEdgeDistB);
				float v3End = triGradNormalizedDist(xy3DistToXRight, v3EndEdgeDistB);
				// linearly interpolate between the two values
				float v1Incr = (v1End - v1Start) / (xRightAccumulator - xLeftAccumulator);
				float v1Current = v1Start + ( v1Incr * std::abs(std::min(unclippedLeftX, 0)) );
				float v2Incr = (v2End - v2Start) / (xRightAccumulator - xLeftAccumulator);
				float v2Current = v2Start + ( v2Incr * std::abs(std::min(unclippedLeftX, 0)) );
				float v3Incr = (v3End - v3Start) / (xRightAccumulator - xLeftAccumulator);
				float v3Current = v3Start + ( v3Incr * std::abs(std::min(unclippedLeftX, 0)) );

				for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
				{
					float perspInterp = ( v1Current * v1PerspMul ) + ( v2Current * v2PerspMul ) + ( v3Current * v3PerspMul );
					perspInterp += 1.0f - ( v1Current + v2Current + v3Current ); // offset to protect from warping (hacky)
					perspInterp = 1.0f / perspInterp;
					float v1CurPersp = v1Current * ( v1PerspMul * perspInterp );
					float v2CurPersp = v2Current * ( v2PerspMul * perspInterp );
					float v3CurPersp = v3Current * ( v3PerspMul * perspInterp );
					float texCoordX = ( v1CurPersp * texCoordX1 ) + ( v2CurPersp * texCoordX2 ) + ( v3CurPersp * texCoordX3 );
					float texCoordY = ( v1CurPersp * texCoordY1 ) + ( v2CurPersp * texCoordY2 ) + ( v3CurPersp * texCoordY3 );
					this->fragmentShader( currentColor, face, m_CurrentTexture, v1CurPersp, v2CurPersp, v3CurPersp, texCoordX,
								texCoordY );
					m_CP->setColor( currentColor );
					m_CP->putPixel( m_FBPixels, m_FBNumPixels, pixel );

					v1Current += v1Incr;
					v2Current += v2Incr;
					v3Current += v3Incr;
				}

				// increment accumulators
				xLeftAccumulator  += xLeftIncrBottom;
				xRightAccumulator += xRightIncrBottom;
			}

			yInRelationToY1 += yInRelationIncr;
		}
	}

	// set the previously used color back since we're done with the gradients
	m_CP->setColor( previousColor );
}
*/

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x4, y4 );
	drawLine( x4, y4, x1, y1 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	drawTriangleFilled( x1, y1, x2, y2, x3, y3 );
	drawTriangleFilled( x1, y1, x4, y4, x3, y3 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawCircleHelper (int originX, int originY, int x, int y, bool filled)
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
	if (x1_3 > (int)width) x1_3 = width;
	if (x2_4 > (int)width) x2_4 = width;
	if (x5_7 > (int)width) x5_7 = width;
	if (x6_8 > (int)width) x6_8 = width;

	int pixel1 = ( (y1_2 * width) + x1_3  );
	int pixel2 = ( (y1_2 * width) + x2_4  );
	int pixel3 = ( (y3_4 * width) + x1_3  );
	int pixel4 = ( (y3_4 * width) + x2_4  );
	int pixel5 = ( (y5_6 * width) + x5_7  );
	int pixel6 = ( (y5_6 * width) + x6_8  );
	int pixel7 = ( (y7_8 * width) + x5_7  );
	int pixel8 = ( (y7_8 * width) + x6_8  );

	if (filled)
	{
		// span from pixel2 to pixel1
		int tempPixel = pixel2;
		if (y1_2 >= 0 && y1_2 < (int)height)
		{
			while (tempPixel < pixel1)
			{
				m_CP.template putPixel<width, height>( m_Pxls, tempPixel );
				tempPixel += 1;
			}
		}

		// span from pixel4 to pixel3
		if (y3_4 >= 0 && y3_4 < (int)height)
		{
			tempPixel = pixel4;
			while (tempPixel < pixel3)
			{
				m_CP.template putPixel<width, height>( m_Pxls, tempPixel );
				tempPixel += 1;
			}
		}

		// span from pixel6 to pixel5
		if (y5_6 >= 0 && y5_6 < (int)height)
		{
			tempPixel = pixel6;
			while (tempPixel < pixel5)
			{
				m_CP.template putPixel<width, height>( m_Pxls, tempPixel );
				tempPixel += 1;
			}
		}

		// span form pixel8 to pixel7
		if (y7_8 >= 0 && y7_8 < (int)height)
		{
			tempPixel = pixel8;
			while (tempPixel < pixel7)
			{
				m_CP.template putPixel<width, height>( m_Pxls, tempPixel );
				tempPixel += 1;
			}
		}
	}
	else
	{
		if (y1_2 >= 0 && y1_2 < (int)height)
		{
			m_CP.template putPixel<width, height>( m_Pxls, pixel1 );
			m_CP.template putPixel<width, height>( m_Pxls, pixel2 );
		}
		if (y3_4 >= 0 && y3_4 < (int)height)
		{
			m_CP.template putPixel<width, height>( m_Pxls, pixel3 );
			m_CP.template putPixel<width, height>( m_Pxls, pixel4 );
		}
		if (y5_6 >= 0 && y5_6 < (int)height)
		{
			m_CP.template putPixel<width, height>( m_Pxls, pixel5 );
			m_CP.template putPixel<width, height>( m_Pxls, pixel6 );
		}
		if (y7_8 >= 0 && y7_8 < (int)height)
		{
			m_CP.template putPixel<width, height>( m_Pxls, pixel7 );
			m_CP.template putPixel<width, height>( m_Pxls, pixel8 );
		}
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawCircle (float originX, float originY, float radius)
{
	// getting the pixel values of the vertices
	int originXUInt = originX * (width  - 1);
	int originYUInt = originY * (height - 1);
	int radiusUInt = radius * (width - 1);

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

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawCircleFilled (float originX, float originY, float radius)
{
	// getting the pixel values of the vertices
	int originXUInt = originX * (width  - 1);
	int originYUInt = originY * (height - 1);
	int radiusUInt = radius * (width - 1);

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

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawText (float xStart, float yStart, const char* text, float scaleFactor)
{
	// TODO text doesn't render if scale factor isn't an integer beyond 1.0f, fix later?
	if ( scaleFactor > 1.0f )
	{
		scaleFactor = std::round( scaleFactor );
	}

	// getting the pixel values of the vertices
	int currentXInt = xStart * (width  - 1);
	int currentYInt = yStart * (height - 1);
	int currentPixel = (currentYInt * width) + currentXInt;

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
		numXPixelsToSkip = std::abs( xStart ) * width;
	}

	// for right border clipping
	int rightBorderPixel = 0;
	if ( yStart >= 0.0f )
	{
		rightBorderPixel = width * ( std::floor(static_cast<float>(currentYInt * width) / width) + 1 );
	}
	else
	{
		rightBorderPixel = -1 * ( std::abs(currentYInt + 1) * width );
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
				rightClipX = rightBorderPixel + ( width * std::floor(row * scaleFactor) );
			}
			else
			{
				rightClipX = rightBorderPixel + ( width * row );
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
						int pixelToWrite = currentPixel + ( width * std::floor(nNCurrentY) );

						if ( pixelOn
								&& pixelToWrite >= 0
								&& pixelToWrite < static_cast<int>( width * height )
								&& xPixelsSkipped >= numXPixelsToSkip
								&& pixelToWrite < rightClipX )
						{
							m_CP.template putPixel<width, height>( m_Pxls, pixelToWrite );
						}

						rightClipX += width;
						nNCurrentY += 1.0f;
						pixelsMovedDown++;
					}

					rightClipX -= ( width * pixelsMovedDown );
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
			currentPixel += ( (width * pixelsMovedDown) - pixelsMovedRight );
			pixelsMovedRight = 0;
		}

		// reset the y-index back to the top, move the x-index to the right of the last char
		currentYInt = yStart * (height - 1);
		currentXInt += static_cast<unsigned int>( characterWidth * scaleFactor );
		currentPixel = ( currentYInt * width ) + currentXInt;
	}
}

/*
template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawSprite (float xStart, float yStart, Sprite& sprite)
{
	// getting the pixel values of the vertices
	int startXInt = xStart * (width - 1);
	int startYInt = yStart * (width - 1);
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
							m_CP->setColor( color );
							m_CP->putPixel( m_FBPixels, m_FBNumPixels, pixelToWrite );
					}

					// TODO this is a smoothbrain way to remove the 'aliasing' that occurs when rotating
					// but it works,.. maybe fix later?
					// TODO update: commenting out for now because of how outrageously smoothbrain it is
					// int sbPixelRight = pixelToWrite + 1;
					// if ( sbPixelRight >= 0 &&  // top clipping
					// 		sbPixelRight < fbSize && // bottom clipping
					// 		xTranslatedBack + 1 >= 0 && // left clipping
					// 		xTranslatedBack + 1 < m_FBWidth && // right clipping
					// 		! (color.m_IsMonochrome && color.m_M == 0.0f) )
					// {
					// 		m_CP->putPixel( m_FBPixels, m_FBNumPixels, sbPixelRight );
					// }
					// int sbPixelDown = pixelToWrite + m_FBWidth;
					// if ( sbPixelDown >= 0 &&  // top clipping
					// 		sbPixelDown < fbSize && // bottom clipping
					// 		xTranslatedBack >= 0 && // left clipping
					// 		xTranslatedBack < m_FBWidth && // right clipping
					// 		! (color.m_IsMonochrome && color.m_M == 0.0f) )
					// {
					// 		m_CP->putPixel( m_FBPixels, m_FBNumPixels, sbPixelDown );
					// }

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
*/

/*
void SoftwareGraphics::testTexture (Texture& texture)
{
	const unsigned int height = texture.getHeight();
	const unsigned int width = texture.getWidth();

	for ( unsigned int row = 0; row < height; row++ )
	{
		for ( unsigned int column = 0; column < width; column++ )
		{
			m_CP->setColor( texture.getColorProfile()->getPixel(texture.getPixels(), width * height, (row * width) + column) );
			m_CP->putPixel( m_FBPixels, m_FBNumPixels, (row * m_FBWidth) + column );
		}
	}
}
*/

#undef m_CP
#undef m_CurrentFont
#undef m_Pxls
#undef m_NumPxls

#endif // SOFTWARE_GRAPHICS_HPP
