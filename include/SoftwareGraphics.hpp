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

// TODO remove after testing
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
		void drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
		void drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
		void drawCircle (float originX, float originY, float radius) override;
		void drawCircleFilled (float originX, float originY, float radius) override;
		void drawText (float xStart, float yStart, const char* text, float scaleFactor) override;

		void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::MONOCHROME_1BIT>& sprite) override;
		void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGBA_32BIT>& sprite) override;
		void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGB_24BIT>& sprite) override;

		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::MONOCHROME_1BIT>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGBA_32BIT>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT>& shaderData) override;

		// TODO remove this after testing
		void testTexture (Texture<CP_FORMAT::RGBA_32BIT>* texture) override;

	protected:
		void drawCircleHelper (int originX, int originY, int x, int y, bool filled = false);

		template <typename S>
		void drawSpriteHelper (float xStart, float yStart, S& sprite);

		template <CP_FORMAT texFormat> void drawTriangleShadedHelper (Face& face, TriShaderData<texFormat>& shaderData);

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

static inline void triSortVertices (int& x1Sorted, int& y1Sorted, float& x1FSorted, float& y1FSorted, float& x1TexCoord, float& y1TexCoord,
					float& v1PerspMul, int& x2Sorted, int& y2Sorted, float& x2FSorted, float& y2FSorted, float& x2TexCoord,
					float& y2TexCoord, float& v2PerspMul, int& x3Sorted, int& y3Sorted, float& x3FSorted, float& y3FSorted,
					float& x3TexCoord, float& y3TexCoord, float& v3PerspMul )
{
	// first sort by y values
	if (y2Sorted > y3Sorted)
	{
		int xTemp = x2Sorted;
		int yTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		float xTexCoordTemp = x2TexCoord;
		float yTexCoordTemp = y2TexCoord;
		float vPerspMulTemp = v2PerspMul;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xTemp;
		y3Sorted = yTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
		x2TexCoord = x3TexCoord;
		y2TexCoord = y3TexCoord;
		x3TexCoord = xTexCoordTemp;
		y3TexCoord = yTexCoordTemp;
		v2PerspMul = v3PerspMul;
		v3PerspMul = vPerspMulTemp;
	}
	if (y1Sorted > y2Sorted)
	{
		int xTemp = x1Sorted;
		int yTemp = y1Sorted;
		float xFTemp = x1FSorted;
		float yFTemp = y1FSorted;
		float xTexCoordTemp = x1TexCoord;
		float yTexCoordTemp = y1TexCoord;
		float vPerspMulTemp = v1PerspMul;
		x1Sorted = x2Sorted;
		y1Sorted = y2Sorted;
		x2Sorted = xTemp;
		y2Sorted = yTemp;
		x1FSorted = x2FSorted;
		y1FSorted = y2FSorted;
		x2FSorted = xFTemp;
		y2FSorted = yFTemp;
		x1TexCoord = x2TexCoord;
		y1TexCoord = y2TexCoord;
		x2TexCoord = xTexCoordTemp;
		y2TexCoord = yTexCoordTemp;
		v1PerspMul = v2PerspMul;
		v2PerspMul = vPerspMulTemp;
	}
	if (y2Sorted > y3Sorted)
	{
		int xTemp = x2Sorted;
		int yTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		float xTexCoordTemp = x2TexCoord;
		float yTexCoordTemp = y2TexCoord;
		float vPerspMulTemp = v2PerspMul;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xTemp;
		y3Sorted = yTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
		x2TexCoord = x3TexCoord;
		y2TexCoord = y3TexCoord;
		x3TexCoord = xTexCoordTemp;
		y3TexCoord = yTexCoordTemp;
		v2PerspMul = v3PerspMul;
		v3PerspMul = vPerspMulTemp;
	}

	// then sort by x values
	if (y2Sorted == y3Sorted && x2Sorted > x3Sorted)
	{
		int xTemp = x2Sorted;
		int yTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		float xTexCoordTemp = x2TexCoord;
		float yTexCoordTemp = y2TexCoord;
		float vPerspMulTemp = v2PerspMul;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xTemp;
		y3Sorted = yTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
		x2TexCoord = x3TexCoord;
		y2TexCoord = y3TexCoord;
		x3TexCoord = xTexCoordTemp;
		y3TexCoord = yTexCoordTemp;
		v2PerspMul = v3PerspMul;
		v3PerspMul = vPerspMulTemp;
	}
	if (y1Sorted == y2Sorted && x1Sorted > x2Sorted)
	{
		int xTemp = x1Sorted;
		int yTemp = y1Sorted;
		float xFTemp = x1FSorted;
		float yFTemp = y1FSorted;
		float xTexCoordTemp = x1TexCoord;
		float yTexCoordTemp = y1TexCoord;
		float vPerspMulTemp = v1PerspMul;
		x1Sorted = x2Sorted;
		y1Sorted = y2Sorted;
		x2Sorted = xTemp;
		y2Sorted = yTemp;
		x1FSorted = x2FSorted;
		y1FSorted = y2FSorted;
		x2FSorted = xFTemp;
		y2FSorted = yFTemp;
		x1TexCoord = x2TexCoord;
		y1TexCoord = y2TexCoord;
		x2TexCoord = xTexCoordTemp;
		y2TexCoord = yTexCoordTemp;
		v1PerspMul = v2PerspMul;
		v2PerspMul = vPerspMulTemp;
	}
	if (y2Sorted == y3Sorted && x2Sorted > x3Sorted)
	{
		int xTemp = x2Sorted;
		int yTemp = y2Sorted;
		float xFTemp = x2FSorted;
		float yFTemp = y2FSorted;
		float xTexCoordTemp = x2TexCoord;
		float yTexCoordTemp = y2TexCoord;
		float vPerspMulTemp = v2PerspMul;
		x2Sorted = x3Sorted;
		y2Sorted = y3Sorted;
		x3Sorted = xTemp;
		y3Sorted = yTemp;
		x2FSorted = x3FSorted;
		y2FSorted = y3FSorted;
		x3FSorted = xFTemp;
		y3FSorted = yFTemp;
		x2TexCoord = x3TexCoord;
		y2TexCoord = y3TexCoord;
		x3TexCoord = xTexCoordTemp;
		y3TexCoord = yTexCoordTemp;
		v2PerspMul = v3PerspMul;
		v3PerspMul = vPerspMulTemp;
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
	float fake = 0.0f;
	triSortVertices( x1Sorted, y1Sorted, x1FSorted, y1FSorted, fake, fake, fake, x2Sorted, y2Sorted, x2FSorted, y2FSorted, fake, fake, fake,
				x3Sorted, y3Sorted, x3FSorted, y3FSorted, fake, fake, fake );

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

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::MONOCHROME_1BIT>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::MONOCHROME_1BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGBA_32BIT>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGBA_32BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGB_24BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
template <CP_FORMAT texFormat>
void SoftwareGraphics<width, height, format, bufferSize>::drawTriangleShadedHelper (Face& face, TriShaderData<texFormat>& shaderData)
{
	Camera3D& camera = shaderData.camera;

	// get previous color, since we'll want to set it back when we're done with the shading colors
	const Color previousColor = m_CP.getColor();

	// a color to store for fragment shading
	Color currentColor = m_CP.getColor();

	// put through the vertex shader first
	( *shaderData.vShader )( shaderData );

	// first determine if this triangle needs to be rendered (back face culling)
	face.calcFaceNormals();
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
	int x1Int = std::ceil( x1 * (width  - 1) );
	int y1Int = std::ceil( y1 * (height - 1) );
	int x2Int = std::ceil( x2 * (width  - 1) );
	int y2Int = std::ceil( y2 * (height - 1) );
	int x3Int = std::ceil( x3 * (width  - 1) );
	int y3Int = std::ceil( y3 * (height - 1) );

	int x1Sorted = x1Int;
	int y1Sorted = y1Int;
	int x2Sorted = x2Int;
	int y2Sorted = y2Int;
	int x3Sorted = x3Int;
	int y3Sorted = y3Int;
	float x1FSorted = static_cast<float>( x1Sorted );
	float y1FSorted = static_cast<float>( y1Sorted );
	float x2FSorted = static_cast<float>( x2Sorted );
	float y2FSorted = static_cast<float>( y2Sorted );
	float x3FSorted = static_cast<float>( x3Sorted );
	float y3FSorted = static_cast<float>( y3Sorted );
	float texCoordX1 = face.vertices[0].texCoords.x();
	float texCoordY1 = face.vertices[0].texCoords.y();
	float texCoordX2 = face.vertices[1].texCoords.x();
	float texCoordY2 = face.vertices[1].texCoords.y();
	float texCoordX3 = face.vertices[2].texCoords.x();
	float texCoordY3 = face.vertices[2].texCoords.y();
	float v1PerspMul = 1.0f / face.vertices[0].vec.w();
	float v2PerspMul = 1.0f / face.vertices[1].vec.w();
	float v3PerspMul = 1.0f / face.vertices[2].vec.w();

	/*
	// TODO test code, remove after
	float v1Cur = 1.0f;
	float v2Cur = 0.0f;
	float v3Cur = 0.0f;
	float perspInterp = ( v1Cur * v1PerspMul ) + ( v2Cur * v2PerspMul ) + ( v3Cur * v3PerspMul );
	perspInterp = v1PerspMul / perspInterp;
	std::cout << "perspInterp: " << std::to_string(perspInterp) << std::endl;
	*/

	// sorting vertices
	triSortVertices( x1Sorted, y1Sorted, x1FSorted, y1FSorted, texCoordX1, texCoordY1, v1PerspMul,
				x2Sorted, y2Sorted, x2FSorted, y2FSorted, texCoordX2, texCoordY2, v2PerspMul,
				x3Sorted, y3Sorted, x3FSorted, y3FSorted, texCoordX3, texCoordY3, v3PerspMul );

	// getting the slope of each line
	const float line1Slope = ((float) y2Sorted - y1Sorted) / ((float) x2Sorted - x1Sorted);
	const float line2Slope = ((float) y3Sorted - y1Sorted) / ((float) x3Sorted - x1Sorted);
	const float line3Slope = ((float) y3Sorted - y2Sorted) / ((float) x3Sorted - x2Sorted);

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

	// gradient calculation vars
	const float oneOverdX = 1.0f / ( ((x2FSorted - x3FSorted) * (y1FSorted - y3FSorted)) - ((x1FSorted - x3FSorted) * (y2FSorted - y3FSorted)) );
	const float oneOverdY = -oneOverdX;
	Vector<3> v1GradVal({ 1.0f, 0.0f, 0.0f });
	Vector<3> v2GradVal({ 0.0f, 1.0f, 0.0f });
	Vector<3> v3GradVal({ 0.0f, 0.0f, 1.0f });
	Vector<3> xGradStep = ( ((v2GradVal - v3GradVal) * (y1FSorted - y3FSorted)) - ((v1GradVal - v3GradVal) * (y2FSorted - y3FSorted)) ) * oneOverdX;
	Vector<3> yGradStep = ( ((v2GradVal - v3GradVal) * (x1FSorted - x3FSorted)) - ((v1GradVal - v3GradVal) * (x2FSorted - x3FSorted)) ) * oneOverdY;
	// so grad vals will be positive
	xGradStep *= -1.0f;
	yGradStep *= -1.0f;
	// TODO these gradients are totally wrong, need to figure it out

	int topHalfRow = y1Sorted;
	while ( topHalfRow < y2Sorted && topHalfRow < 0 )
	{
		// even if off screen, we still need to increment xLeftAccumulator and xRightAccumulator
		xLeftAccumulator  += xLeftIncrTop;
		xRightAccumulator += xRightIncrTop;

		topHalfRow++;
	}

	// render up until the second vertice
	for (int row = topHalfRow; row < y2Sorted && row < height; row++)
	{
		// rounding the points and clipping horizontally
		const int unclippedLeftX = std::ceil( xLeftAccumulator );
		const int unclippedRightX = std::ceil( xRightAccumulator );
		const unsigned int leftX  = std::min( std::max((int)unclippedLeftX, 0), (int)width - 1 );
		const unsigned int rightX = std::max( std::min((int)unclippedRightX, (int)width - 1), 0 );

		const unsigned int tempXY1 = ( (row * width) + leftX  );
		const unsigned int tempXY2 = ( (row * width) + rightX );

		/*
		const float oneOverPixelStride = 1.0f / ( static_cast<float>( rightX ) - static_cast<float>( leftX ) );
		const float rowF = static_cast<float>( row );
		const float leftXF = static_cast<float>( leftX );
		const float rightXF = static_cast<float>( rightX );
		const float v1CurStart = (xGradStep.at(0) * (leftXF - x1FSorted)) + (yGradStep.at(0) * (rowF - y1FSorted));
		const float v1CurEnd   = (xGradStep.at(0) * (rightXF - x1FSorted)) + (yGradStep.at(0) * (rowF - y1FSorted));
		const float v2CurStart = (xGradStep.at(1) * (leftXF - x2FSorted)) + (yGradStep.at(1) * (rowF - y2FSorted));
		const float v2CurEnd   = (xGradStep.at(1) * (rightXF - x2FSorted)) + (yGradStep.at(1) * (rowF - y2FSorted));
		const float v3CurStart = (xGradStep.at(2) * (leftXF - x3FSorted)) + (yGradStep.at(2) * (rowF - y3FSorted));
		const float v3CurEnd   = (xGradStep.at(2) * (rightXF - x3FSorted)) + (yGradStep.at(2) * (rowF - y3FSorted));
		const float perspCorrectStart = 1.0f / ( (v1CurStart * v1PerspMul) + (v2CurStart * v2PerspMul) + (v3CurStart * v3PerspMul) );
		const float perspCorrectEnd   = 1.0f / ( (v1CurEnd * v1PerspMul) + (v2CurEnd * v2PerspMul) + (v3CurEnd * v3PerspMul) );
		const float v1CurPerspStart = ( v1CurStart * v1PerspMul * perspCorrectStart );
		const float v1CurPerspEnd   = ( v1CurEnd * v1PerspMul * perspCorrectEnd );
		const float v2CurPerspStart = ( v2CurStart * v2PerspMul * perspCorrectStart );
		const float v2CurPerspEnd   = ( v2CurEnd * v2PerspMul * perspCorrectEnd );
		const float v3CurPerspStart = ( v3CurStart * v3PerspMul * perspCorrectStart );
		const float v3CurPerspEnd   = ( v3CurEnd * v3PerspMul * perspCorrectEnd );
		const float texCoordXStart  = ( v1CurPerspStart * texCoordX1 ) + ( v2CurPerspStart * texCoordX2 ) + ( v3CurPerspStart * texCoordX3 );
		const float texCoordYStart  = ( v1CurPerspStart * texCoordY1 ) + ( v2CurPerspStart * texCoordY2 ) + ( v3CurPerspStart * texCoordY3 );
		const float texCoordXEnd    = ( v1CurPerspEnd * texCoordX1 ) + ( v2CurPerspEnd * texCoordX2 ) + ( v3CurPerspEnd * texCoordX3 );
		const float texCoordYEnd    = ( v1CurPerspEnd * texCoordY1 ) + ( v2CurPerspEnd * texCoordY2 ) + ( v3CurPerspEnd * texCoordY3 );
		const float v1CurPerspStep  = ( v1CurPerspEnd - v1CurPerspStart ) * oneOverPixelStride;
		const float v2CurPerspStep  = ( v2CurPerspEnd - v2CurPerspStart ) * oneOverPixelStride;
		const float v3CurPerspStep  = ( v3CurPerspEnd - v3CurPerspStart ) * oneOverPixelStride;
		const float texCoordXStep   = ( texCoordXEnd - texCoordXStart ) * oneOverPixelStride;
		const float texCoordYStep   = ( texCoordYEnd - texCoordYStart ) * oneOverPixelStride;
		float v1CurPersp = v1CurPerspStart;
		float v2CurPersp = v2CurPerspStart;
		float v3CurPersp = v3CurPerspStart;
		float texCoordX = texCoordXStart;
		float texCoordY = texCoordYStart;

		for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
		{
			( *shaderData.fShader )( currentColor, shaderData, v1CurPersp, v2CurPersp, v3CurPersp, texCoordX, texCoordY );
			m_CP.setColor( currentColor );
			m_CP.template putPixel<width, height>( m_Pxls, pixel );
			v1CurPersp += v1CurPerspStep;
			v2CurPersp += v2CurPerspStep;
			v3CurPersp += v3CurPerspStep;
			texCoordX += texCoordXStep;
			texCoordY += texCoordYStep;
		}
		*/

		const float oneOverPixelStride = 1.0f / ( static_cast<float>( rightX ) - static_cast<float>( leftX ) );
		const float rowF = static_cast<float>( row );
		const float leftXF = static_cast<float>( leftX );
		const float rightXF = static_cast<float>( rightX );
		const float v1CurStart = (xGradStep.at(0) * (leftXF - x1FSorted)) + (yGradStep.at(0) * (rowF - y1FSorted));
		const float v1CurEnd   = (xGradStep.at(0) * (rightXF - x1FSorted)) + (yGradStep.at(0) * (rowF - y1FSorted));
		const float v2CurStart = (xGradStep.at(1) * (leftXF - x2FSorted)) + (yGradStep.at(1) * (rowF - y2FSorted));
		const float v2CurEnd   = (xGradStep.at(1) * (rightXF - x2FSorted)) + (yGradStep.at(1) * (rowF - y2FSorted));
		const float v3CurStart = (xGradStep.at(2) * (leftXF - x3FSorted)) + (yGradStep.at(2) * (rowF - y3FSorted));
		const float v3CurEnd   = (xGradStep.at(2) * (rightXF - x3FSorted)) + (yGradStep.at(2) * (rowF - y3FSorted));
		const float v1CurStep  = ( v1CurEnd - v1CurStart ) * oneOverPixelStride;
		const float v2CurStep  = ( v2CurEnd - v2CurStart ) * oneOverPixelStride;
		const float v3CurStep  = ( v3CurEnd - v3CurStart ) * oneOverPixelStride;
		float v1Cur = v1CurStart;
		float v2Cur = v2CurStart;
		float v3Cur = v3CurStart;

		for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
		{
			std::cout << "v1Cur: " << std::to_string(1.0f - v1Cur) << "   v2Cur: " << std::to_string(1.0f - v2Cur) << "   v3Cur: " << std::to_string(1.0f - v3Cur) << std::endl;
			float v1CurAdj = 1.0f - v1Cur;
			float v2CurAdj = 1.0f - v2Cur;
			float v3CurAdj = 1.0f - v3Cur;
			float perspInterp = (( v1CurAdj * v1PerspMul ) + ( v2CurAdj * v2PerspMul ) + ( v3CurAdj * v3PerspMul ));
			perspInterp = 1.0f / perspInterp;
			float v1CurPersp = v1CurAdj * v1PerspMul * perspInterp;
			float v2CurPersp = v2CurAdj * v2PerspMul * perspInterp;
			float v3CurPersp = v3CurAdj * v3PerspMul *perspInterp;
			float texCoordX = ( v1CurPersp * texCoordX1 ) + ( v2CurPersp * texCoordX2 ) + ( v3CurPersp * texCoordX3 );
			float texCoordY = ( v1CurPersp * texCoordY1 ) + ( v2CurPersp * texCoordY2 ) + ( v3CurPersp * texCoordY3 );
			( *shaderData.fShader )( currentColor, shaderData, v1CurPersp, v2CurPersp, v3CurPersp, texCoordX, texCoordY);
			m_CP.setColor( currentColor );
			m_CP.template putPixel<width, height>( m_Pxls, pixel );
			v1Cur += v1CurStep;
			v2Cur += v2CurStep;
			v3Cur += v3CurStep;
		}

		// increment accumulators
		xLeftAccumulator  += xLeftIncrTop;
		xRightAccumulator += xRightIncrTop;
	}

	// in case the top of the triangle is straight, set the accumulators appropriately
	if ( y1Sorted == y2Sorted )
	{
		xLeftAccumulator  = (float) x1Sorted;
		xRightAccumulator = (float) x2Sorted;
	}

	int bottomHalfRow = y2Sorted;
	while ( bottomHalfRow < y3Sorted && bottomHalfRow < 0 )
	{
		// even if off screen, we still need to increment xLeftAccumulator and xRightAccumulator
		xLeftAccumulator  += xLeftIncrBottom;
		xRightAccumulator += xRightIncrBottom;

		bottomHalfRow++;
	}

	// rasterize up until the last vertice
	for (int row = bottomHalfRow; row <= y3Sorted && row < height; row++)
	{
		// rounding the points and clipping horizontally
		int unclippedLeftX = std::ceil( xLeftAccumulator );
		int unclippedRightX = std::ceil( xRightAccumulator );
		unsigned int leftX  = std::min( std::max((int)unclippedLeftX, 0), (int)width - 1 );
		unsigned int rightX = std::max( std::min((int)unclippedRightX, (int)width - 1), 0 );

		unsigned int tempXY1 = ( (row * width) + leftX  );
		unsigned int tempXY2 = ( (row * width) + rightX );

		const float oneOverPixelStride = 1.0f / ( static_cast<float>( rightX ) - static_cast<float>( leftX ) );
		const float rowF = static_cast<float>( row );
		const float leftXF = static_cast<float>( leftX );
		const float rightXF = static_cast<float>( rightX );
		const float v1CurStart = (xGradStep.at(0) * (leftXF - x1FSorted)) + (yGradStep.at(0) * (rowF - y1FSorted));
		const float v1CurEnd   = (xGradStep.at(0) * (rightXF - x1FSorted)) + (yGradStep.at(0) * (rowF - y1FSorted));
		const float v2CurStart = (xGradStep.at(1) * (leftXF - x2FSorted)) + (yGradStep.at(1) * (rowF - y2FSorted));
		const float v2CurEnd   = (xGradStep.at(1) * (rightXF - x2FSorted)) + (yGradStep.at(1) * (rowF - y2FSorted));
		const float v3CurStart = (xGradStep.at(2) * (leftXF - x3FSorted)) + (yGradStep.at(2) * (rowF - y3FSorted));
		const float v3CurEnd   = (xGradStep.at(2) * (rightXF - x3FSorted)) + (yGradStep.at(2) * (rowF - y3FSorted));
		const float v1CurStep  = ( v1CurEnd - v1CurStart ) * oneOverPixelStride;
		const float v2CurStep  = ( v2CurEnd - v2CurStart ) * oneOverPixelStride;
		const float v3CurStep  = ( v3CurEnd - v3CurStart ) * oneOverPixelStride;
		float v1Cur = v1CurStart;
		float v2Cur = v2CurStart;
		float v3Cur = v3CurStart;

		for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
		{
			float v1CurAdj = 1.0f - v1Cur;
			float v2CurAdj = 1.0f - v2Cur;
			float v3CurAdj = 1.0f - v3Cur;
			float perspInterp = (( v1CurAdj * v1PerspMul ) + ( v2CurAdj * v2PerspMul ) + ( v3CurAdj * v3PerspMul ));
			perspInterp = 1.0f / perspInterp;
			float v1CurPersp = v1CurAdj * v1PerspMul * perspInterp;
			float v2CurPersp = v2CurAdj * v2PerspMul * perspInterp;
			float v3CurPersp = v3CurAdj * v3PerspMul *perspInterp;
			float texCoordX = ( v1CurPersp * texCoordX1 ) + ( v2CurPersp * texCoordX2 ) + ( v3CurPersp * texCoordX3 );
			float texCoordY = ( v1CurPersp * texCoordY1 ) + ( v2CurPersp * texCoordY2 ) + ( v3CurPersp * texCoordY3 );
			( *shaderData.fShader )( currentColor, shaderData, v1CurPersp, v2CurPersp, v3CurPersp, texCoordX, texCoordY);
			m_CP.setColor( currentColor );
			m_CP.template putPixel<width, height>( m_Pxls, pixel );
			v1Cur += v1CurStep;
			v2Cur += v2CurStep;
			v3Cur += v3CurStep;
		}

		// increment accumulators
		xLeftAccumulator  += xLeftIncrBottom;
		xRightAccumulator += xRightIncrBottom;
	}

	// set the previously used color back since we're done with the gradients
	m_CP.setColor( previousColor );
}

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

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::MONOCHROME_1BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::MONOCHROME_1BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGBA_32BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGBA_32BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGB_24BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGB_24BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
template <typename S>
void SoftwareGraphics<width, height, format, bufferSize>::drawSpriteHelper (float xStart, float yStart, S& sprite)
{
	// getting the pixel values of the vertices
	int startXInt = xStart * (width - 1);
	int startYInt = yStart * (width - 1);
	int currentXInt = startXInt;
	int currentYInt = startYInt;

	const int spriteWidth = sprite.getWidth();
	const int spriteHeight = sprite.getHeight();

	float scaleFactor = sprite.getScaleFactor();

	float spriteRowTravel = 1.0f;
	float spritePixelTravel = 1.0f;
	if ( scaleFactor < 1.0f ) // we need to skip rows and columns if downscaling
	{
		spriteRowTravel = spriteRowTravel / scaleFactor;
		spritePixelTravel = spritePixelTravel / scaleFactor;
	}
	const unsigned int scaledHeight = sprite.getScaledHeight();
	const unsigned int scaledWidth  = sprite.getScaledWidth();
	const float nNXTravel = static_cast<float>( scaledWidth ) / static_cast<float>( spriteWidth ); // nearest neighbor scaling vars
	const float nNYTravel = static_cast<float>( scaledHeight ) / static_cast<float>( spriteHeight );
	float nNCurrentX = 0.0f; // these vars keep a 'running total' for upscaling
	float nNCurrentY = 0.0f;
	float nNYLeftOver = 0.0f;

	// for rotation
	int spriteRotDegrees = sprite.getRotationAngle();
	int spriteRotPointX = std::round( sprite.getRotationPointX() * scaleFactor );;
	int spriteRotPointY = std::round( sprite.getRotationPointY() * scaleFactor );

	// for bottom clipping
	const int fbSize = width * height;

	for ( float row = 0; row < spriteHeight; row += spriteRowTravel )
	{
		unsigned int pixelsMovedRight = 0;
		unsigned int pixelsMovedDown = 0;
		unsigned int xPixelsSkipped = 0;

		for ( float pixel = 0; pixel < spriteWidth; pixel += spritePixelTravel )
		{
			Color color = sprite.getColor( static_cast<unsigned int>(std::floor(pixel)), static_cast<unsigned int>(std::floor(row)) );

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

					int pixelToWrite = (yTranslatedBack * width) + xTranslatedBack;

					if ( pixelToWrite >= 0 &&  // top clipping
							pixelToWrite < fbSize && // bottom clipping
							xTranslatedBack >= 0 && // left clipping
							xTranslatedBack < width && // right clipping
							! (color.m_IsMonochrome && color.m_M == 0.0f) && // monochrome transparency
							! (color.m_A == 0.0f) // color transparency
							)
					{
							m_CP.setColor( color );
							m_CP.template putPixel<width, height>( m_Pxls, pixelToWrite );
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

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
void SoftwareGraphics<width, height, format, bufferSize>::testTexture (Texture<CP_FORMAT::RGBA_32BIT>* texture)
{
	const unsigned int texHeight = texture->getHeight();
	const unsigned int texWidth = texture->getWidth();

	for ( unsigned int row = 0; row < texHeight; row++ )
	{
		for ( unsigned int column = 0; column < texWidth; column++ )
		{
			m_CP.setColor( texture->getColor(column, row) );
			m_CP.template putPixel<width, height>( m_Pxls, (row * width) + column );
		}
	}
}

#undef m_CP
#undef m_CurrentFont
#undef m_Pxls
#undef m_NumPxls

#endif // SOFTWARE_GRAPHICS_HPP
