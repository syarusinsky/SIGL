#include "SoftwareGraphics.hpp"

#include "Font.hpp"
#include "Sprite.hpp"

#include <math.h>
#include <algorithm>


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
		m_ColorProfile->putPixel( m_FBPixels, pixel );
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
			if (slope < 1.0f) // stride along x-axis across multiple pixels
			{
				while (xAccumulator < 1.0f && pixel <= pixelEnd)
				{
					m_ColorProfile->putPixel( m_FBPixels, pixel );

					xAccumulator += slope;
					pixel += 1;
				}
				xAccumulator = xAccumulator - 1.0f;
			}
			else // stride along x-axis by one pixel
			{
				m_ColorProfile->putPixel( m_FBPixels, pixel );

				pixel += 1;
			}

			// y stride
			if (slope >= 1.0f) // stride along the y-axis across multiple pixels
			{
				while (yAccumulator >= 1.0f && pixel <= pixelEnd)
				{
					m_ColorProfile->putPixel( m_FBPixels, pixel );

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
			if (slope > -1.0f) // stride along x-axis across multiple pixels
			{
				while (xAccumulator > -1.0f && pixel <= pixelEnd)
				{
					m_ColorProfile->putPixel( m_FBPixels, pixel );

					xAccumulator += slope;
					pixel -= 1;
				}
				xAccumulator = xAccumulator + 1.0f;
			}
			else // stride along x-axis by one pixel
			{
				m_ColorProfile->putPixel( m_FBPixels, pixel );

				pixel -= 1;
			}

			// y stride
			if (slope <= -1.0f) // stride along the y-axis across multiple pixels
			{
				while (yAccumulator <= 1.0f && pixel <= pixelEnd)
				{
					m_ColorProfile->putPixel( m_FBPixels, pixel );

					yAccumulator += 1.0f;
					pixel += m_FBWidth;
				}
				yAccumulator += slope;
			}
			else // stride along the y-axis by one pixel
			{
				pixel += m_FBWidth;

				m_ColorProfile->putPixel( m_FBPixels, pixel );
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
			m_ColorProfile->putPixel( m_FBPixels, rowPixel );
		}
	}
}

void SoftwareGraphics::drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x1, y1 );
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

	// xLeftIncrBottom < xRightIncrBottom is a substitute for line2Slope being on the top or bottom, is this correct???
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
	if (line1Slope == 0.0f)
	{
		// fill row to x2, y2
		float tempX1 = x1FSorted;
		float tempY1 = y1FSorted;
		float tempX2 = x2FSorted;
		float tempY2 = y2FSorted;

		// if after clipping this line exists within the screen, render the line
		if ( clipLine(&tempX1, &tempY1, &tempX2, &tempY2) )
		{
			unsigned int tempX1UInt = x1FSorted * (m_FBWidth  - 1);
			unsigned int tempY1UInt = y1FSorted * (m_FBHeight - 1);
			unsigned int tempX2UInt = x2FSorted * (m_FBWidth  - 1);
			unsigned int tempY2UInt = y2FSorted * (m_FBHeight - 1);

			unsigned int tempXY1 = ( (tempY1UInt * m_FBWidth) + tempX1UInt );
			unsigned int tempXY2 = ( (tempY2UInt * m_FBWidth) + tempX2UInt );

			for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
			{
				m_ColorProfile->putPixel( m_FBPixels, pixel );
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
				// get x-intercepts of lines
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

				// rounding the points and clipping horizontally
				unsigned int leftX  = std::min( std::max((int)std::round(xLeftAccumulator), 0), (int)m_FBWidth - 1 );
				unsigned int rightX = std::max( std::min((int)std::round(xRightAccumulator), (int)m_FBWidth - 1), 0 );

				// ensuring the accumulators don't overrun the edges of the triangle
				if (row == y2Sorted) leftX = x2Sorted;
				if (row == y3Sorted) rightX = x2Sorted;

				unsigned int tempXY1 = ( (row * m_FBWidth) + leftX  );
				unsigned int tempXY2 = ( (row * m_FBWidth) + rightX );

				for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
				{
					m_ColorProfile->putPixel( m_FBPixels, pixel );
				}
			}
			else // even if off screen, we still need to increment xLeftAccumulator and xRightAccumulator
			{
				// get x-intercepts of lines
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
		for (int row = y2Sorted; row <= y3Sorted; row++)
		{
			// clip vertically if row is off screen
			if (row >= (int)m_FBHeight)
			{
				break;
			}
			else if ( row >= 0 )
			{
				// get x-intercepts of lines
				xLeftAccumulator  += xLeftIncrBottom;
				xRightAccumulator += xRightIncrBottom;

				// rounding the points and clipping horizontally
				unsigned int leftX  = std::min( std::max((int)std::round(xLeftAccumulator), 0), (int)m_FBWidth - 1 );
				unsigned int rightX = std::max( std::min((int)std::round(xRightAccumulator), (int)m_FBWidth - 1), 0 );

				unsigned int tempXY1 = ( (row * m_FBWidth) + leftX  );
				unsigned int tempXY2 = ( (row * m_FBWidth) + rightX );

				for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
				{
					m_ColorProfile->putPixel( m_FBPixels, pixel );
				}
			}
		}
	}
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
				m_ColorProfile->putPixel( m_FBPixels, tempPixel );
				tempPixel += 1;
			}
		}

		// span from pixel4 to pixel3
		if (y3_4 >= 0 && y3_4 < (int)m_FBHeight)
		{
			tempPixel = pixel4;
			while (tempPixel < pixel3)
			{
				m_ColorProfile->putPixel( m_FBPixels, tempPixel );
				tempPixel += 1;
			}
		}

		// span from pixel6 to pixel5
		if (y5_6 >= 0 && y5_6 < (int)m_FBHeight)
		{
			tempPixel = pixel6;
			while (tempPixel < pixel5)
			{
				m_ColorProfile->putPixel( m_FBPixels, tempPixel );
				tempPixel += 1;
			}
		}

		// span form pixel8 to pixel7
		if (y7_8 >= 0 && y7_8 < (int)m_FBHeight)
		{
			tempPixel = pixel8;
			while (tempPixel < pixel7)
			{
				m_ColorProfile->putPixel( m_FBPixels, tempPixel );
				tempPixel += 1;
			}
		}
	}
	else
	{
		if (y1_2 >= 0 && y1_2 < (int)m_FBHeight)
		{
			m_ColorProfile->putPixel( m_FBPixels, pixel1 );
			m_ColorProfile->putPixel( m_FBPixels, pixel2 );
		}
		if (y3_4 >= 0 && y3_4 < (int)m_FBHeight)
		{
			m_ColorProfile->putPixel( m_FBPixels, pixel3 );
			m_ColorProfile->putPixel( m_FBPixels, pixel4 );
		}
		if (y5_6 >= 0 && y5_6 < (int)m_FBHeight)
		{
			m_ColorProfile->putPixel( m_FBPixels, pixel5 );
			m_ColorProfile->putPixel( m_FBPixels, pixel6 );
		}
		if (y7_8 >= 0 && y7_8 < (int)m_FBHeight)
		{
			m_ColorProfile->putPixel( m_FBPixels, pixel7 );
			m_ColorProfile->putPixel( m_FBPixels, pixel8 );
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

void SoftwareGraphics::drawText (float xStart, float yStart, std::string text, float scaleFactor)
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
	unsigned int charIndex = 0;
	for( char& character : text )
	{
		unsigned int charPixelIndex = m_CurrentFont->getCharacterIndex( character ) * characterWidth;
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
							m_ColorProfile->putPixel( m_FBPixels, pixelToWrite );
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
		charIndex++;
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
			Color color = spriteCP->getPixel( spritePixels, (std::floor(row) * spriteWidth) + std::floor(pixel) );

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
							m_ColorProfile->putPixel( m_FBPixels, pixelToWrite );
					}

					// TODO this is a smoothbrain way to remove the 'aliasing' that occurs when rotating
					// but it works,.. maybe fix later?
					int sbPixelRight = pixelToWrite + 1;
					if ( sbPixelRight >= 0 &&  // top clipping
							sbPixelRight < fbSize && // bottom clipping
							xTranslatedBack + 1 >= 0 && // left clipping
							xTranslatedBack + 1 < m_FBWidth && // right clipping
							! (color.m_IsMonochrome && color.m_M == 0.0f) )
					{
							m_ColorProfile->putPixel( m_FBPixels, sbPixelRight );
					}
					int sbPixelDown = pixelToWrite + m_FBWidth;
					if ( sbPixelDown >= 0 &&  // top clipping
							sbPixelDown < fbSize && // bottom clipping
							xTranslatedBack >= 0 && // left clipping
							xTranslatedBack < m_FBWidth && // right clipping
							! (color.m_IsMonochrome && color.m_M == 0.0f) )
					{
							m_ColorProfile->putPixel( m_FBPixels, sbPixelDown );
					}

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


