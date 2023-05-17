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

#include "IGraphics.hpp"

#include "Font.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"
#include "Engine3D.hpp"

#include <algorithm>
#include <limits>

// just to avoid compilation error
template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class SoftwareGraphicsNo3D : public IGraphics<width, height, format, api, include3D, shaderPassDataSize>
{
	public:
		virtual ~SoftwareGraphicsNo3D() {}
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class SoftwareGraphics3D : public IGraphics<width, height, format, api, include3D, shaderPassDataSize>
{
	public:
		virtual ~SoftwareGraphics3D() {}

		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData) override;
		void drawDepthBuffer (Camera3D& camera) override;

	protected:
		template <CP_FORMAT texFormat>
		inline void renderScanlines (int startRow, int endRowExclusive, float x1, float y1,
			float& xLeftAccumulator, float& xRightAccumulator, float v1PerspMul, float v1Depth, float xLeftIncr, float xRightIncr,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData, Color& currentColor, float texCoordX1, float texCoordY1,
			float texCoordXXIncr, float texCoordXYIncr, float texCoordYXIncr, float texCoordYYIncr, float perspXIncr, float perspYIncr,
			float depthXIncr, float depthYIncr, float v1LightAmnt, float lightAmntXIncr, float lightAmntYIncr);
		template <CP_FORMAT texFormat> void drawTriangleShadedHelper (Face& face, TriShaderData<texFormat, shaderPassDataSize>& shaderData);

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::clip;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::lerp;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::percentageBetween;

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_ColorProfile;
		using IGraphics3D<width, height, shaderPassDataSize>::m_DepthBuffer;
		using IGraphics3D<width, height, shaderPassDataSize>::m_ShaderPassData;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_FB;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class SoftwareGraphics 	: public std::conditional<include3D, SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>,
								SoftwareGraphicsNo3D<width, height, format, api, include3D, shaderPassDataSize>>::type
{
	// only a surface should be able to construct
	template<RENDER_API rAPI, unsigned int w, unsigned int h, CP_FORMAT f, unsigned int nT, bool i3D, unsigned int sPDS> friend class SurfaceThreaded;
	template<RENDER_API rAPI, unsigned int w, unsigned int h, CP_FORMAT f, bool i3D, unsigned int sPDS> friend class SurfaceSingleCore;

	public:
		void setColor (float r, float g, float b) override;
		void setColor (bool val) override;

		void setFont (Font* font) override;
		Font* getFont() override;

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

	protected:
		// assumes this line is clipped by the clipEdge
		inline void computeIntersection (const float clipEdge, bool horizontal, const float tempX1, const float tempY1,
						const float tempX2, const float tempY2, float& intersectionX, float& intersectionY);

		void drawTriangleFilledHelper (float x1, float y1, float x2, float y2, float x3, float y3);
		void drawCircleHelper (int originX, int originY, int x, int y, bool filled = false);
		template <typename S>
		void drawSpriteHelper (float xStart, float yStart, S& sprite);

		SoftwareGraphics();
		virtual ~SoftwareGraphics() override;

		void startFrame() override;
		void endFrame() override;

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::clip;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::lerp;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::percentageBetween;

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_ColorProfile;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_CurrentFont;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_FB;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::SoftwareGraphics()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::~SoftwareGraphics()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::setColor (float r, float g, float b)
{
	m_ColorProfile.setColor( r, g, b );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::setColor (bool val)
{
	m_ColorProfile.setColor( val );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::setFont (Font* font)
{
	m_CurrentFont = font;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
Font* SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::getFont()
{
	return m_CurrentFont;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::startFrame()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::endFrame()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::fill()
{
	for ( unsigned int pixelNum = 0; pixelNum < width * height; pixelNum++ )
	{
		m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixelNum );
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawLine (float xStart, float yStart, float xEnd, float yEnd)
{
	// clip line and return if line is off screen
	if ( !IGraphics<width, height, format, api, include3D, shaderPassDataSize>::clipLine( &xStart, &yStart, &xEnd, &yEnd ) ) return;

	const unsigned int xStartUInt = std::ceil( xStart * (width  - 1) );
	const unsigned int yStartUInt = std::ceil( yStart * (height - 1) );
	const unsigned int xEndUInt   = std::ceil( xEnd   * (width  - 1) );
	const unsigned int yEndUInt   = std::ceil( yEnd   * (height - 1) );

	const float slope = ((float) yEndUInt - yStartUInt) / ((float)xEndUInt - xStartUInt);

	unsigned int pixelStart = ( (width * yStartUInt) + xStartUInt );
	unsigned int pixelEnd   = ( (width * yEndUInt  ) + xEndUInt   );

	if ( pixelStart > pixelEnd )
	{
		// swap
		const unsigned int temp = pixelStart;
		pixelStart = pixelEnd;
		pixelEnd   = temp;
	}

	unsigned int pixel = pixelStart;
	float yAccumulator = 0.0f;

	if ( xStartUInt == xEndUInt ) // vertical line
	{
		while ( pixel <= pixelEnd )
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
			pixel += width;
		}
	}
	else if ( yStartUInt == yEndUInt ) // horizontal line
	{
		while ( pixel <= pixelEnd )
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
			pixel += 1;
		}
	}
	else if ( slope > 0.0f && slope <= 1.0f )
	{
		while ( pixel <= pixelEnd )
		{
			// stride x axis
			while ( yAccumulator < 1.0f && pixel <= pixelEnd )
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
				pixel += 1;

				yAccumulator += slope;
			}

			pixel += width;
			yAccumulator = std::fmod( yAccumulator, 1.0f );
		}
	}
	else if ( slope < 0.0f && slope >= -1.0f )
	{
		while ( pixel <= pixelEnd )
		{
			// stride x axis
			while ( yAccumulator < 1.0f && pixel <= pixelEnd )
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
				pixel -= 1;

				yAccumulator -= slope;
			}

			pixel += width;
			yAccumulator = std::fmod( yAccumulator, 1.0f );
		}
	}
	else if ( slope > 1.0f )
	{
		yAccumulator = slope;

		// stride y axis
		while ( pixel <= pixelEnd )
		{
			while ( yAccumulator >= 1.0f && pixel <= pixelEnd )
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
				pixel += width;

				yAccumulator -= 1.0f;
			}

			pixel += 1;
			yAccumulator += slope;
		}
	}
	else if ( slope < -1.0f )
	{
		yAccumulator = slope;

		// stride y axis
		while ( pixel <= pixelEnd )
		{
			while ( yAccumulator <= -1.0f && pixel <= pixelEnd )
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
				pixel += width;

				yAccumulator += 1.0f;
			}

			pixel -= 1;
			yAccumulator += slope;
		}
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawBox (float xStart, float yStart, float xEnd, float yEnd)
{
	drawLine( xStart, yStart, xEnd,   yStart );
	drawLine( xEnd,   yStart, xEnd,   yEnd   );
	drawLine( xEnd,   yEnd,   xStart, yEnd   );
	drawLine( xStart, yEnd,   xStart, yStart );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd)
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
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), rowPixel );
		}
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x1, y1 );
}


template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleFilledHelper (float x1, float y1, float x2, float y2,
													float x3, float y3)
{
	// getting the pixel values of the vertices
	const int x1UInt = x1 * ( width  - 1 );
	const int y1UInt = y1 * ( height - 1 );
	const int x2UInt = x2 * ( width  - 1 );
	const int y2UInt = y2 * ( height - 1 );
	const int x3UInt = x3 * ( width  - 1 );
	const int y3UInt = y3 * ( height - 1 );

	int x1Sorted = x1UInt;
	int y1Sorted = y1UInt;
	int x2Sorted = x2UInt;
	int y2Sorted = y2UInt;
	int x3Sorted = x3UInt;
	int y3Sorted = y3UInt;
	float x1FSorted = static_cast<float>( x1UInt );
	float y1FSorted = static_cast<float>( y1UInt );
	float x2FSorted = static_cast<float>( x2UInt );
	float y2FSorted = static_cast<float>( y2UInt );
	float x3FSorted = static_cast<float>( x3UInt );
	float y3FSorted = static_cast<float>( y3UInt );

	// sorting vertices
	triSortVertices( x1Sorted, y1Sorted, x1FSorted, y1FSorted,
				x2Sorted, y2Sorted, x2FSorted, y2FSorted,
				x3Sorted, y3Sorted, x3FSorted, y3FSorted );

	// getting the slope of each line
	const float line1Slope = ( y2FSorted - y1FSorted ) / ( x2FSorted - x1FSorted );
	const float line2Slope = ( y3FSorted - y1FSorted ) / ( x3FSorted - x1FSorted );
	const float line3Slope = ( y3FSorted - y2FSorted ) / ( x3FSorted - x2FSorted );

	// floats for x-intercepts (assuming the top of the triangle is pointed for now)
	float xLeftAccumulator  = x1FSorted;
	float xRightAccumulator = x1FSorted;

	// floats for incrementing xLeftAccumulator and xRightAccumulator
	float xLeftIncrTop     = 1.0f / line1Slope;
	float xRightIncrTop    = 1.0f / line2Slope;
	float xLeftIncrBottom  = 1.0f / line3Slope;
	float xRightIncrBottom = 1.0f / line2Slope;

	// xLeftIncrBottom < xRightIncrBottom is a substitute for line2Slope being on the top or bottom
	const bool needsSwapping = ( xLeftIncrBottom < xRightIncrBottom );

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
	if ( line1Slope == 0.0f )
	{
		xRightAccumulator = x2FSorted;

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
			xRightAccumulator = x3FSorted;
		}

		const unsigned int tempX1Int = tempX1;
		const unsigned int tempY1Int = tempY1;
		const unsigned int tempX2Int = tempX2;
		const unsigned int tempY2Int = tempY2;

		const unsigned int tempXY1 = ( (tempY1Int * width) + tempX1Int );
		const unsigned int tempXY2 = ( (tempY2Int * width) + tempX2Int );

		for ( unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1 )
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
		}
	}
	else
	{
		// render up until the second vertice
		for ( int row = y1Sorted; row <= y2Sorted; row++ )
		{
			// rounding the points and clipping horizontally
			const unsigned int leftX  = xLeftAccumulator;
			const unsigned int rightX = xRightAccumulator;

			const unsigned int tempXY1 = ( (row * width) + leftX  );
			const unsigned int tempXY2 = ( (row * width) + rightX );

			for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
			}

			// increment accumulators
			xLeftAccumulator  += xLeftIncrTop;
			xRightAccumulator += xRightIncrTop;
		}
	}

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

	// rasterize up until the last vertice
	if (y2Sorted != y3Sorted) // if the bottom of the triangle isn't a horizontal line
	{
		for (int row = y2Sorted + 1; row <= y3Sorted; row++)
		{
			// rounding the points and clipping horizontally
			const unsigned int leftX  = xLeftAccumulator;
			const unsigned int rightX = xRightAccumulator;

			const unsigned int tempXY1 = ( (row * width) + leftX  );
			const unsigned int tempXY2 = ( (row * width) + rightX );

			for (unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1)
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
			}

			// increment accumulators
			xLeftAccumulator  += xLeftIncrBottom;
			xRightAccumulator += xRightIncrBottom;
		}
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
inline void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::computeIntersection(
		const float clipEdge, bool horizontal, const float tempX1, const float tempY1,
		const float tempX2, const float tempY2, float& intersectionX, float& intersectionY )
{
	if ( horizontal )
	{
		const float perc = percentageBetween( clipEdge, tempX1, tempX2 );
		const float lerpedY = lerp( perc, tempY1, tempY2 );

		intersectionX = clipEdge;
		intersectionY = lerpedY;
	}
	else // vertical
	{
		const float perc = percentageBetween( clipEdge, tempY1, tempY2 );
		const float lerpedX = lerp( perc, tempX1, tempX2 );

		intersectionX = lerpedX;
		intersectionY = clipEdge;
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleFilled (float x1, float y1, float x2, float y2,
													float x3, float y3)
{
	float vertXArr[3] = { x1, x2, x3 };
	float vertYArr[3] = { y1, y2, y3 };
	// TODO replace these vectors with stack based lists
	std::vector<std::pair<float, float>> outVertices;
	for ( unsigned int vertNum = 0; vertNum < 3; vertNum++ )
	{
		outVertices.emplace_back( vertXArr[vertNum], vertYArr[vertNum] );
	}

	// clip triangle into sub-triangles
	const float clipEdges[2] = { 0.0f, 1.0f };
	for ( unsigned int edgeNum = 0; edgeNum < 2; edgeNum++ )
	{
		// clip x
		std::vector<std::pair<float, float>> newVertices = outVertices;
		outVertices.clear();

		for ( unsigned int vertNum = 0; vertNum < newVertices.size(); vertNum++ )
		{
			const float tempX1 = newVertices[vertNum].first;
			const float tempY1 = newVertices[vertNum].second;
			const float tempX2 = newVertices[(vertNum + 1) % newVertices.size()].first;
			const float tempY2 = newVertices[(vertNum + 1) % newVertices.size()].second;

			const bool tempX1Inside = ( edgeNum == 0 ) ? tempX1 >= 0.0f : tempX1 <= 1.0f;
			const bool tempX2Inside = ( edgeNum == 0 ) ? tempX2 >= 0.0f : tempX2 <= 1.0f;
			if ( tempX2Inside )
			{
				if ( ! tempX1Inside )
				{
					// add intersection
					float intersectionX;
					float intersectionY;
					computeIntersection( clipEdges[edgeNum], true, tempX1, tempY1, tempX2, tempY2,
								intersectionX, intersectionY );
					outVertices.emplace_back( intersectionX, intersectionY );
				}

				outVertices.emplace_back( tempX2, tempY2 );
			}
			else if ( tempX1Inside )
			{
				// add intersection
				float intersectionX;
				float intersectionY;
				computeIntersection( clipEdges[edgeNum], true, tempX1, tempY1, tempX2, tempY2, intersectionX, intersectionY );
				outVertices.emplace_back( intersectionX, intersectionY );
			}
		}

		// clip y
		newVertices = outVertices;
		outVertices.clear();

		for ( unsigned int vertNum = 0; vertNum < newVertices.size(); vertNum++ )
		{
			const float tempX1 = newVertices[vertNum].first;
			const float tempY1 = newVertices[vertNum].second;
			const float tempX2 = newVertices[(vertNum + 1) % newVertices.size()].first;
			const float tempY2 = newVertices[(vertNum + 1) % newVertices.size()].second;

			const bool tempY1Inside = ( edgeNum == 0 ) ? tempY1 >= 0.0f : tempY1 <= 1.0f;
			const bool tempY2Inside = ( edgeNum == 0 ) ? tempY2 >= 0.0f : tempY2 <= 1.0f;
			if ( tempY2Inside )
			{
				if ( ! tempY1Inside )
				{
					// add intersection
					float intersectionX;
					float intersectionY;
					computeIntersection( clipEdges[edgeNum], false, tempX1, tempY1, tempX2, tempY2,
								intersectionX, intersectionY );
					outVertices.emplace_back( intersectionX, intersectionY );
				}

				outVertices.emplace_back( tempX2, tempY2 );
			}
			else if ( tempY1Inside )
			{
				// add intersection
				float intersectionX;
				float intersectionY;
				computeIntersection( clipEdges[edgeNum], false, tempX1, tempY1, tempX2, tempY2, intersectionX, intersectionY );
				outVertices.emplace_back( intersectionX, intersectionY );
			}
		}
	}

	if ( outVertices.size() > 2 )
	{
		for ( unsigned int vertNum = 1; vertNum < outVertices.size() - 1; vertNum++ )
		{
			const float newX1 = outVertices[0].first;
			const float newY1 = outVertices[0].second;
			const float newX2 = outVertices[vertNum].first;
			const float newY2 = outVertices[vertNum].second;
			const float newX3 = outVertices[vertNum + 1].first;
			const float newY3 = outVertices[vertNum + 1].second;

			drawTriangleFilledHelper( newX1, newY1, newX2, newY2,newX3, newY3 );
		}
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics3D<width, height, format, api,  include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::MONOCHROME_1BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGBA_32BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGB_24BIT>( face, shaderData );
}

inline float calcIncr(Vector<3>& values, float xy1, float xy2, float xy3, float oneOverdXY)
{
	const float retVal = ( ((values.at(1) - values.at(2)) * (xy1 - xy3)) - ((values.at(0) - values.at(2)) * (xy2 - xy3)) ) * oneOverdXY;
	return ( retVal > 100000.0f || retVal < -100000.0f ) ? 0.0f : retVal;
}

inline float saturate (float val)
{
	if ( val > 1.0f ) return 1.0f; else if ( val < 0.0f ) return 0.0f; else return val;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <CP_FORMAT texFormat>
inline void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::renderScanlines (int startRow, int endRowExclusive, float x1,
			float y1, float& xLeftAccumulator, float& xRightAccumulator, float v1PerspMul, float v1Depth, float xLeftIncr, float xRightIncr,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData, Color& currentColor, float texCoordX1, float texCoordY1,
			float texCoordXXIncr, float texCoordXYIncr, float texCoordYXIncr, float texCoordYYIncr, float perspXIncr, float perspYIncr,
			float depthXIncr, float depthYIncr, float v1LightAmnt, float lightAmntXIncr, float lightAmntYIncr)
{
	// TODO this can be optimized by quite a bit, and likely will need to be
	for (int row = startRow; row < endRowExclusive && row < height; row++)
	{
		// rounding the points and clipping horizontally
		const int unclippedLeftX = std::floor( xLeftAccumulator );
		const int unclippedRightX = std::floor( xRightAccumulator );
		const unsigned int leftX  = std::min( std::max((int)unclippedLeftX, 0), (int)width - 1 );
		const unsigned int rightX = std::max( std::min((int)unclippedRightX, (int)width - 1), 0 );

		const unsigned int tempXY1 = ( (row * width) + leftX  );
		const unsigned int tempXY2 = ( (row * width) + rightX );
		const float oneOverPixelStride = 1.0f / ( static_cast<float>( rightX + 1) - static_cast<float>( leftX ) );
		const float rowF = static_cast<float>( row );
		const float leftXF  = std::min( std::max(xLeftAccumulator, 0.0f), width - 1.0f );
		const float rightXF = std::max( std::min(xRightAccumulator, width - 1.0f), 0.0f );
		const float depthStart = v1Depth + ( depthYIncr * (rowF - y1) ) + ( depthXIncr * (leftXF - x1) );
		const float depthEnd   = v1Depth + ( depthYIncr * (rowF - y1) ) + ( depthXIncr * (rightXF - x1) );
		const float texXStart  = ( texCoordX1 * v1PerspMul ) + ( texCoordXYIncr * (rowF - y1) ) + ( texCoordXXIncr * (leftXF - x1) );
		const float texXEnd    = ( texCoordX1 * v1PerspMul ) + ( texCoordXYIncr * (rowF - y1) ) + ( texCoordXXIncr * (rightXF - x1) );
		const float texYStart  = ( texCoordY1 * v1PerspMul ) + ( texCoordYYIncr * (rowF - y1) ) + ( texCoordYXIncr * (leftXF - x1) );
		const float texYEnd    = ( texCoordY1 * v1PerspMul ) + ( texCoordYYIncr * (rowF - y1) ) + ( texCoordYXIncr * (rightXF - x1) );
		const float persStart  = v1PerspMul + ( perspYIncr * (rowF - y1) ) + ( perspXIncr * (leftXF - x1) );
		const float persEnd    = v1PerspMul + ( perspYIncr * (rowF - y1) ) + ( perspXIncr * (rightXF - x1) );
		const float lightStart = v1LightAmnt + ( lightAmntYIncr * (rowF - y1) ) + ( lightAmntXIncr * (leftXF - x1) );
		const float lightEnd   = v1LightAmnt + ( lightAmntYIncr * (rowF - y1) ) + ( lightAmntXIncr * (rightXF - x1) );
		const float depthIncr  = ( depthEnd - depthStart ) * oneOverPixelStride;
		const float persIncr   = ( persEnd - persStart ) * oneOverPixelStride;
		const float texXIncr   = ( texXEnd - texXStart ) * oneOverPixelStride;
		const float texYIncr   = ( texYEnd - texYStart ) * oneOverPixelStride;
		const float lightIncr  = ( lightEnd - lightStart ) * oneOverPixelStride;
		float depth = depthStart;
		float texX  = texXStart;
		float texY  = texYStart;
		float pers  = persStart;
		float light = lightStart;

		for (unsigned int pixel = tempXY1; pixel <= tempXY2; pixel += 1)
		{
			if ( m_DepthBuffer[pixel] > depth )
			{
				const float perspOffset = 1.0f / pers;
				const float texCoordX = texX * perspOffset;
				const float texCoordY = texY * perspOffset;
				( *shaderData.fShader )( currentColor, shaderData, 0.0f, 0.0f, 0.0f, texCoordX, texCoordY, light );
				m_ColorProfile.setColor( currentColor );
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );

				m_DepthBuffer[pixel] = depth;
			}

			depth += depthIncr;
			texX  += texXIncr;
			texY  += texYIncr;
			pers  += persIncr;
			light += lightIncr;
		}

		// increment accumulators
		xLeftAccumulator  += xLeftIncr;
		xRightAccumulator += xRightIncr;
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <CP_FORMAT texFormat>
void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShadedHelper (Face& face,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData)
{
	// setup shader data
	shaderData.shaderPassData = m_ShaderPassData;
	Camera3D& camera = shaderData.camera;

	// TODO this is not how clipping should be done, need to do homogenous clipping in the future
	const float nearClipDepth = camera.getNearClip();
	const float farClipDepth = camera.getFarClip();

	// clip based near plane or far plane
	if ( face.vertices[0].vec.z() < nearClipDepth || face.vertices[0].vec.z() > farClipDepth
			|| face.vertices[1].vec.z() < nearClipDepth || face.vertices[1].vec.z() > farClipDepth
			|| face.vertices[2].vec.z() < nearClipDepth || face.vertices[2].vec.z() > farClipDepth ) return;

	// get previous color, since we'll want to set it back when we're done with the shading colors
	const Color previousColor = m_ColorProfile.getColor();

	// a color to store for fragment shading
	Color currentColor = m_ColorProfile.getColor();

	// put through the vertex shader first
	( *shaderData.vShader )( shaderData );

	// TODO camera projection should be done in the vertex shader?
	camera.projectFace( face );
	camera.scaleXYToZeroToOne( face );

	// backface culling
	const Vector<4>& vertexVec = face.vertices[0].vec;
	const Vector<4>& normal = face.calcFaceNormals();
	if ( ! (normal.x() * (vertexVec.x() - camera.x())
			+ normal.y() * (vertexVec.y() - camera.y())
			+ normal.z() * (vertexVec.z() - camera.z()) > 0.0f) )
	{
		return;
	}

	// sorting vertices
	triSortVertices( face.vertices[0], face.vertices[1], face.vertices[2], width, height );

	float x1 = face.vertices[0].vec.x();
	float y1 = face.vertices[0].vec.y();
	float x2 = face.vertices[1].vec.x();
	float y2 = face.vertices[1].vec.y();
	float x3 = face.vertices[2].vec.x();
	float y3 = face.vertices[2].vec.y();

	int x1Ceil = std::ceil( x1 );
	int y1Ceil = std::ceil( y1 );
	int x2Ceil = std::ceil( x2 );
	int y2Ceil = std::ceil( y2 );
	int x3Ceil = std::ceil( x3 );
	int y3Ceil = std::ceil( y3 );
	float x1FCeil = static_cast<float>( x1Ceil );
	float y1FCeil = static_cast<float>( y1Ceil );
	float x2FCeil = static_cast<float>( x2Ceil );
	float y2FCeil = static_cast<float>( y2Ceil );
	float x3FCeil = static_cast<float>( x3Ceil );
	float y3FCeil = static_cast<float>( y3Ceil );
	float texCoordX1 = face.vertices[0].texCoords.x();
	float texCoordY1 = face.vertices[0].texCoords.y();
	float texCoordX2 = face.vertices[1].texCoords.x();
	float texCoordY2 = face.vertices[1].texCoords.y();
	float texCoordX3 = face.vertices[2].texCoords.x();
	float texCoordY3 = face.vertices[2].texCoords.y();
	float v1PerspMul = 1.0f / face.vertices[0].vec.w();
	float v2PerspMul = 1.0f / face.vertices[1].vec.w();
	float v3PerspMul = 1.0f / face.vertices[2].vec.w();
	float v1Depth = face.vertices[0].vec.z();
	float v2Depth = face.vertices[1].vec.z();
	float v3Depth = face.vertices[2].vec.z();
	Vector<4> lightDir({-0.5f, -0.5f, 0.0f, 0.0f}); // TODO remove this after testing
	float v1LightAmnt = saturate( face.vertices[0].normal.dotProduct(lightDir) ) * 0.8f + 0.2f;
	float v2LightAmnt = saturate( face.vertices[1].normal.dotProduct(lightDir) ) * 0.8f + 0.2f;
	float v3LightAmnt = saturate( face.vertices[2].normal.dotProduct(lightDir) ) * 0.8f + 0.2f;

	// floats for x-intercepts (assuming the top of the triangle is pointed for now)
	float xLeftAccumulator  = x1FCeil;
	float xRightAccumulator = x1FCeil;

	// floats for incrementing xLeftAccumulator and xRightAccumulator
	float xLeftIncrTop     = ( x2FCeil - x1FCeil ) / ( y2FCeil - y1FCeil );
	float xRightIncrTop    = ( x3FCeil - x1FCeil ) / ( y3FCeil - y1FCeil );
	float xLeftIncrBottom  = ( x3FCeil - x2FCeil ) / ( y3FCeil - y2FCeil );
	float xRightIncrBottom = ( x3FCeil - x1FCeil ) / ( y3FCeil - y1FCeil );

	// xLeftIncrBottom < xRightIncrBottom is a substitute for line2Slope being on the top or bottom
	bool needsSwapping = ( xLeftIncrBottom < xRightIncrBottom );

	// depending on the position of the vertices, we need to swap increments
	if ( (needsSwapping && x1Ceil < x2Ceil) || (needsSwapping && x1Ceil >= x2Ceil && x2Ceil > x3Ceil) )
	{
		float tempIncr = xLeftIncrTop;
		xLeftIncrTop = xRightIncrTop;
		xRightIncrTop = tempIncr;

		tempIncr = xLeftIncrBottom;
		xLeftIncrBottom = xRightIncrBottom;
		xRightIncrBottom = tempIncr;
	}

	// gradient calculation vars
	const float oneOverdX = 1.0f / ( ((x2FCeil - x3FCeil) * (y1FCeil - y3FCeil)) - ((x1FCeil - x3FCeil) * (y2FCeil - y3FCeil)) );
	const float oneOverdY = -oneOverdX;
	Vector<3> texCoordsX({ texCoordX1 * v1PerspMul, texCoordX2 * v2PerspMul, texCoordX3 * v3PerspMul });
	Vector<3> texCoordsY({ texCoordY1 * v1PerspMul, texCoordY2 * v2PerspMul, texCoordY3 * v3PerspMul });
	const float texCoordXXIncr = calcIncr( texCoordsX, y1FCeil, y2FCeil, y3FCeil, oneOverdX );
	const float texCoordXYIncr = calcIncr( texCoordsX, x1FCeil, x2FCeil, x3FCeil, oneOverdY );
	const float texCoordYXIncr = calcIncr( texCoordsY, y1FCeil, y2FCeil, y3FCeil, oneOverdX );
	const float texCoordYYIncr = calcIncr( texCoordsY, x1FCeil, x2FCeil, x3FCeil, oneOverdY );
	Vector<3> persps({ v1PerspMul, v2PerspMul, v3PerspMul });
	const float perspXIncr = calcIncr( persps, y1FCeil, y2FCeil, y3FCeil, oneOverdX );
	const float perspYIncr = calcIncr( persps, x1FCeil, x2FCeil, x3FCeil, oneOverdY );
	Vector<3> depths({ v1Depth, v2Depth, v3Depth });
	const float depthXIncr = calcIncr( depths, y1FCeil, y2FCeil, y3FCeil, oneOverdX );
	const float depthYIncr = calcIncr( depths, x1FCeil, x2FCeil, x3FCeil, oneOverdY );
	Vector<3> lightAmnts({ v1LightAmnt, v2LightAmnt, v3LightAmnt });
	const float lightAmntXIncr = calcIncr( lightAmnts, y1FCeil, y2FCeil, y3FCeil, oneOverdX );
	const float lightAmntYIncr = calcIncr( lightAmnts, x1FCeil, x2FCeil, x3FCeil, oneOverdY );

	int topHalfRow = y1Ceil;
	while ( topHalfRow < y2Ceil && topHalfRow < 0 )
	{
		// even if off screen, we still need to increment xLeftAccumulator and xRightAccumulator
		xLeftAccumulator  += xLeftIncrTop;
		xRightAccumulator += xRightIncrTop;

		topHalfRow++;
	}

	// render up until the second vertice
	renderScanlines<texFormat>( topHalfRow, y2Ceil, x1FCeil, y1FCeil, xLeftAccumulator, xRightAccumulator, v1PerspMul, v1Depth, xLeftIncrTop,
			xRightIncrTop, shaderData, currentColor, texCoordX1, texCoordY1, texCoordXXIncr, texCoordXYIncr, texCoordYXIncr, texCoordYYIncr,
			perspXIncr, perspYIncr, depthXIncr, depthYIncr, v1LightAmnt, lightAmntXIncr, lightAmntYIncr );

	// in case the top of the triangle is straight, set the accumulators appropriately
	if ( y1Ceil == y2Ceil == y3Ceil )
	{
		xLeftAccumulator  = x1FCeil;
		xRightAccumulator = x3FCeil;
	}
	else if ( y1Ceil == y2Ceil )
	{
		xLeftAccumulator  = x1FCeil;
		xRightAccumulator = x2FCeil;
	}

	int bottomHalfRow = y2Ceil;
	while ( bottomHalfRow < y3Ceil && bottomHalfRow < 0 )
	{
		// even if off screen, we still need to increment xLeftAccumulator and xRightAccumulator
		xLeftAccumulator  += xLeftIncrBottom;
		xRightAccumulator += xRightIncrBottom;

		bottomHalfRow++;
	}

	// rasterize up until the last vertice
	renderScanlines<texFormat>( bottomHalfRow, y3Ceil + 1, x1FCeil, y1FCeil, xLeftAccumulator, xRightAccumulator, v1PerspMul, v1Depth, xLeftIncrBottom,
			xRightIncrBottom, shaderData, currentColor, texCoordX1, texCoordY1, texCoordXXIncr, texCoordXYIncr, texCoordYXIncr, texCoordYYIncr,
			perspXIncr, perspYIncr, depthXIncr, depthYIncr, v1LightAmnt, lightAmntXIncr, lightAmntYIncr );

	// set the previously used color back since we're done with the gradients
	m_ColorProfile.setColor( previousColor );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawQuad (float x1, float y1, float x2, float y2, float x3, float y3,
		float x4, float y4)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x4, y4 );
	drawLine( x4, y4, x1, y1 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawQuadFilled (float x1, float y1, float x2, float y2, float x3,
		float y3, float x4, float y4)
{
	drawTriangleFilled( x1, y1, x2, y2, x3, y3 );
	drawTriangleFilled( x1, y1, x4, y4, x3, y3 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircleHelper (int originX, int originY, int x, int y, bool filled)
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
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), tempPixel );
				tempPixel += 1;
			}
		}

		// span from pixel4 to pixel3
		if (y3_4 >= 0 && y3_4 < (int)height)
		{
			tempPixel = pixel4;
			while (tempPixel < pixel3)
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), tempPixel );
				tempPixel += 1;
			}
		}

		// span from pixel6 to pixel5
		if (y5_6 >= 0 && y5_6 < (int)height)
		{
			tempPixel = pixel6;
			while (tempPixel < pixel5)
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), tempPixel );
				tempPixel += 1;
			}
		}

		// span form pixel8 to pixel7
		if (y7_8 >= 0 && y7_8 < (int)height)
		{
			tempPixel = pixel8;
			while (tempPixel < pixel7)
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), tempPixel );
				tempPixel += 1;
			}
		}
	}
	else
	{
		if (y1_2 >= 0 && y1_2 < (int)height)
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel1 );
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel2 );
		}
		if (y3_4 >= 0 && y3_4 < (int)height)
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel3 );
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel4 );
		}
		if (y5_6 >= 0 && y5_6 < (int)height)
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel5 );
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel6 );
		}
		if (y7_8 >= 0 && y7_8 < (int)height)
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel7 );
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel8 );
		}
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircle (float originX, float originY, float radius)
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

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircleFilled (float originX, float originY, float radius)
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

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawText (float xStart, float yStart, const char* text, float scaleFactor)
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
							m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixelToWrite );
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

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::MONOCHROME_1BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::MONOCHROME_1BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::RGBA_32BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGBA_32BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGB_24BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGB_24BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <typename S>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSpriteHelper (float xStart, float yStart, S& sprite)
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
							m_ColorProfile.setColor( color );
							m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixelToWrite );
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
					// 		m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, sbPixelRight );
					// }
					// int sbPixelDown = pixelToWrite + m_FBWidth;
					// if ( sbPixelDown >= 0 &&  // top clipping
					// 		sbPixelDown < fbSize && // bottom clipping
					// 		xTranslatedBack >= 0 && // left clipping
					// 		xTranslatedBack < m_FBWidth && // right clipping
					// 		! (color.m_IsMonochrome && color.m_M == 0.0f) )
					// {
					// 		m_ColorProfile->putPixel( m_FBPixels, m_FBNumPixels, sbPixelDown );
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

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawDepthBuffer (Camera3D& camera)
{
	// get previous color, since we'll want to set it back when we're done with the shading colors
	const Color previousColor = m_ColorProfile.getColor();

	Color color;
	const float mul = 1.0f / ( camera.getFarClip() - camera.getNearClip() );
	for ( unsigned int pixelNum = 0; pixelNum < width * height; pixelNum++ )
	{
		float depthVal = 1.0f - ( m_DepthBuffer[pixelNum] * mul );
		color.m_R = depthVal;
		color.m_G = depthVal;
		color.m_B = depthVal;
		m_ColorProfile.setColor( color );
		m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixelNum );
	}

	// set the previously used color back since we're done with the gradients
	m_ColorProfile.setColor( previousColor );
}

#endif // SOFTWAREGRAPHICS_HPP
