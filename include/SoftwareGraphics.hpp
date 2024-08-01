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

#include <functional>
#include <algorithm>
#include <limits>

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class SoftwareGraphicsBase : public IGraphics<width, height, format, api, include3D, shaderPassDataSize>
{
	public:
		virtual ~SoftwareGraphicsBase() {}

	protected:
		template <CP_FORMAT texFormat, bool withTransparency = false>
		inline void drawTriangleShadedHelper (Face& face, TriShaderData<texFormat, shaderPassDataSize>& shaderData,
				std::array<float, width * height>& depthBuffer );
		template <CP_FORMAT texFormat, bool withTransparency = false>
		inline void renderScanlines (int startRow, int endRowExclusive, float x1, float y1,
			float& xLeftAccumulator, float& xRightAccumulator, float v1PerspMul, float v1Depth, float xLeftIncr, float xRightIncr,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData, Color& currentColor, float texCoordX1, float texCoordY1,
			float texCoordXXIncr, float texCoordXYIncr, float texCoordYXIncr, float texCoordYYIncr, float perspXIncr,
			float perspYIncr, float depthXIncr, float depthYIncr, float v1LightAmnt, float lightAmntXIncr, float lightAmntYIncr,
			std::array<float, width * height>& depthBuffer, int leftHanded);
		template <CP_FORMAT texFormat, bool withTransparency = false>
		inline void renderInBoundsTriangle (Face& face, TriShaderData<texFormat, shaderPassDataSize>& shaderData,
							std::array<float, width * height>& depthBuffer);

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_ColorProfile;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_FB;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class SoftwareGraphicsNo3D : public SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>
{
	public:
		virtual ~SoftwareGraphicsNo3D() {}
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class SoftwareGraphics3D : public SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>
{
	public:
		virtual ~SoftwareGraphics3D() {}

		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData) override;
		void drawDepthBuffer (Camera3D& camera) override;
		void clearDepthBuffer() override;

	protected:
		std::array<float, width * height> 		m_DepthBuffer;

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::approxEqual;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::clip;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::lerp;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::percentageBetween;

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_ColorProfile;
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
		template <CP_FORMAT texFormat>
		void drawSpriteHelper (float xStart, float yStart, Sprite<texFormat>& sprite);

		SoftwareGraphics();
		virtual ~SoftwareGraphics() override;

		void startFrame() override;
		void endFrame() override;

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::approxEqual;
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
			yAccumulator -= 1.0f;
		}
	}
	else if ( slope < 0.0f && slope >= -1.0f )
	{
		while ( pixel <= pixelEnd )
		{
			// stride x axis
			while ( yAccumulator < 1.0f && pixel <= pixelEnd - static_cast<unsigned int>(1.0f / -slope) )
			{
				m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
				pixel -= 1;

				yAccumulator -= slope;
			}

			pixel += width;
			yAccumulator -= 1.0f;
		}
	}
	else if ( slope > 1.0f )
	{
		yAccumulator = slope;

		// stride y axis
		while ( pixel <= pixelEnd )
		{
			while ( yAccumulator > 1.0f && pixel <= pixelEnd )
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
			while ( yAccumulator < -1.0f && pixel <= pixelEnd )
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
	xStartUInt = std::min( std::max(xStartUInt, 0), (int)(width - 1 ) );
	yStartUInt = std::min( std::max(yStartUInt, 0), (int)(height - 1) );
	xEndUInt   = std::min( std::max(xEndUInt,   0), (int)(width - 1 ) );
	yEndUInt   = std::min( std::max(yEndUInt,   0), (int)(height - 1) );

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
	// offset to screen space
	x1 *= (width  - 1);
	y1 *= (height - 1);
	x2 *= (width  - 1);
	y2 *= (height - 1);
	x3 *= (width  - 1);
	y3 *= (height - 1);

	triSortVertices( x1, y1, x2, y2, x3, y3 );

	const int y1Ceil = std::ceil( y1 );
	const int y2Ceil = std::ceil( y2 );
	const int y3Ceil = std::ceil( y3 );

	// floats for incrementing xLeftAccumulator and xRightAccumulator
	float xLeftIncrTop     = ( x2 - x1 ) / ( y2 - y1 );
	float xRightIncrTop    = ( x3 - x1 ) / ( y3 - y1 );
	float xLeftIncrBottom  = ( x3 - x2 ) / ( y3 - y2 );
	float xRightIncrBottom = ( x3 - x1 ) / ( y3 - y1 );

	// get handedness by calculating area by getting cross product of vectors of lines 1 and 2
	const float vec1X = x3 - x1;
	const float vec1Y = y3 - y1;
	const float vec2X = x2 - x1;
	const float vec2Y = y2 - y1;

	const float area = ( vec1X * vec2Y ) - ( vec2X * vec1Y );
	const int leftHanded = ( area >= 0 ) ? 1 : 0;

	// depending on the position of the vertices, we need to swap increments
	if ( leftHanded == 0 )
	{
		float tempIncr = xLeftIncrTop;
		xLeftIncrTop = xRightIncrTop;
		xRightIncrTop = tempIncr;

		tempIncr = xLeftIncrBottom;
		xLeftIncrBottom = xRightIncrBottom;
		xRightIncrBottom = tempIncr;
	}

	// floats for x-intercepts (assuming the top of the triangle is pointed for now)
	float xLeftAccumulator  = x1 + ( (static_cast<float>(y1Ceil) - y1) * xLeftIncrTop );
	float xRightAccumulator = x1 + ( (static_cast<float>(y1Ceil) - y1) * xRightIncrTop );

	// render up until the second vertice
	for ( unsigned int row = y1Ceil; row < y2Ceil && row < height; row++ )
	{
		unsigned int leftX =  std::ceil( xLeftAccumulator );
		unsigned int rightX = std::ceil( xRightAccumulator );

		const unsigned int tempXY1 = ( (row * width) + leftX );
		const unsigned int tempXY2 = ( (row * width) + rightX );

		for ( unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1 )
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
		}

		// increment accumulators
		xLeftAccumulator  += xLeftIncrTop;
		xRightAccumulator += xRightIncrTop;
	}

	if ( y1Ceil != y2Ceil ) // need to account for vert 2 being in the middle of both left and right increments
	{
		xLeftAccumulator  += ( (static_cast<float>(y2) - y2Ceil) * xLeftIncrTop )  + ( (static_cast<float>(y2Ceil) - y2) * xLeftIncrBottom );
		xRightAccumulator += ( (static_cast<float>(y2) - y2Ceil) * xRightIncrTop ) + ( (static_cast<float>(y2Ceil) - y2) * xRightIncrBottom );
	}
	else if ( ! floatsAreEqual(y1, y2) ) // still need to account for vert 2 being in the middle of both left and right increments
	{
		xLeftAccumulator  = x1 + ( (y2 - y1) * xLeftIncrTop )  + ( (static_cast<float>(y2Ceil) - y2) * xLeftIncrBottom );
		xRightAccumulator = x1 + ( (y2 - y1) * xRightIncrTop ) + ( (static_cast<float>(y2Ceil) - y2) * xRightIncrBottom );
	}
	else
	{
		xLeftAccumulator = x1 + ( (static_cast<float>(y2Ceil) - y2) * xLeftIncrBottom );
		xRightAccumulator = x2 + ( (static_cast<float>(y2Ceil) - y2) * xRightIncrBottom );
	}

	// rasterize up until the last vertice
	for ( unsigned int row = y2Ceil; row < y3Ceil && row < height; row++ )
	{
		unsigned int leftX =  std::ceil( xLeftAccumulator );
		unsigned int rightX = std::ceil( xRightAccumulator );

		const unsigned int tempXY1 = ( (row * width) + leftX );
		const unsigned int tempXY2 = ( (row * width) + rightX );

		for ( unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1 )
		{
			m_ColorProfile.template putPixel<width, height>( m_FB.getPixels(), pixel );
		}

		// increment accumulators
		xLeftAccumulator  += xLeftIncrBottom;
		xRightAccumulator += xRightIncrBottom;
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
	constexpr unsigned int maxPossibleVerts = 6;
	std::pair<float, float> outVertices[maxPossibleVerts] = { {x1, y1}, {x2, y2}, {x3, y3}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} };
	unsigned int outVerticesSize = 3;

	// clip triangle into sub-triangles
	const float clipEdges[2] = { 0.0f, 1.0f };
	for ( unsigned int edgeNum = 0; edgeNum < 2; edgeNum++ )
	{
		// clip x
		std::pair<float, float> newVertices[maxPossibleVerts];
		unsigned int newVerticesSize = outVerticesSize;
		std::copy( std::begin(outVertices), std::end(outVertices), std::begin(newVertices) );
		outVerticesSize = 0;

		for ( unsigned int vertNum = 0; vertNum < newVerticesSize; vertNum++ )
		{
			const float tempX1 = newVertices[vertNum].first;
			const float tempY1 = newVertices[vertNum].second;
			const float tempX2 = newVertices[(vertNum + 1) % newVerticesSize].first;
			const float tempY2 = newVertices[(vertNum + 1) % newVerticesSize].second;

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
					outVertices[outVerticesSize].first = intersectionX;
					outVertices[outVerticesSize].second = intersectionY;
					outVerticesSize++;
				}

				outVertices[outVerticesSize].first = tempX2;
				outVertices[outVerticesSize].second = tempY2;
				outVerticesSize++;
			}
			else if ( tempX1Inside )
			{
				// add intersection
				float intersectionX;
				float intersectionY;
				computeIntersection( clipEdges[edgeNum], true, tempX1, tempY1, tempX2, tempY2, intersectionX, intersectionY );
				outVertices[outVerticesSize].first = intersectionX;
				outVertices[outVerticesSize].second = intersectionY;
				outVerticesSize++;
			}
		}

		// clip y
		newVerticesSize = outVerticesSize;
		std::copy( std::begin(outVertices), std::end(outVertices), std::begin(newVertices) );
		outVerticesSize = 0;

		for ( unsigned int vertNum = 0; vertNum < newVerticesSize; vertNum++ )
		{
			const float tempX1 = newVertices[vertNum].first;
			const float tempY1 = newVertices[vertNum].second;
			const float tempX2 = newVertices[(vertNum + 1) % newVerticesSize].first;
			const float tempY2 = newVertices[(vertNum + 1) % newVerticesSize].second;

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
					outVertices[outVerticesSize].first = intersectionX;
					outVertices[outVerticesSize].second = intersectionY;
					outVerticesSize++;
				}

				outVertices[outVerticesSize].first = tempX2;
				outVertices[outVerticesSize].second = tempY2;
				outVerticesSize++;
			}
			else if ( tempY1Inside )
			{
				// add intersection
				float intersectionX;
				float intersectionY;
				computeIntersection( clipEdges[edgeNum], false, tempX1, tempY1, tempX2, tempY2, intersectionX, intersectionY );
				outVertices[outVerticesSize].first = intersectionX;
				outVertices[outVerticesSize].second = intersectionY;
				outVerticesSize++;
			}
		}
	}

	if ( outVerticesSize > 2 )
	{
		for ( unsigned int vertNum = 1; vertNum < outVerticesSize - 1; vertNum++ )
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
	SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
		drawTriangleShadedHelper<CP_FORMAT::MONOCHROME_1BIT>( face, shaderData, m_DepthBuffer );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData)
{
	SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
		drawTriangleShadedHelper<CP_FORMAT::RGBA_32BIT>( face, shaderData, m_DepthBuffer );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData)
{
	SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
		drawTriangleShadedHelper<CP_FORMAT::RGB_24BIT>( face, shaderData, m_DepthBuffer );
}

inline float calcIncr(const Vector<3>& values, float xy1, float xy2, float xy3, float oneOverdXY)
{
	const float retVal = ( ((values.at(1) - values.at(2)) * (xy1 - xy3)) - ((values.at(0) - values.at(2)) * (xy2 - xy3)) ) * oneOverdXY;
	return ( retVal > 100000.0f || retVal < -100000.0f ) ? 0.0f : retVal;
}

inline float saturate (float val)
{
	if ( val > 1.0f ) return 1.0f; else if ( val < 0.0f ) return 0.0f; else return val;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize,
	CP_FORMAT texFormat, bool withTransparency>
inline void renderScanlinesHelper (int startRow, int endRowExclusive, float x1, float y1, float& xLeftAccumulator, float& xRightAccumulator,
					float v1PerspMul, float v1Depth, float xLeftIncr, float xRightIncr,
					TriShaderData<texFormat, shaderPassDataSize>& shaderData, Color& currentColor, float texCoordX1,
					float texCoordY1, float texCoordXXIncr, float texCoordXYIncr, float texCoordYXIncr, float texCoordYYIncr,
					float perspXIncr, float perspYIncr, float depthXIncr, float depthYIncr, float v1LightAmnt,
					float lightAmntXIncr, float lightAmntYIncr, std::array<float, width * height>& depthBuffer,
					ColorProfile<format>& colorProfile, FrameBufferFixed<width, height, format, api>& fb, int leftHanded)
{
	for ( unsigned int row = startRow; row < endRowExclusive && row < height; row++ )
	{
		const unsigned int leftX  = std::ceil( xLeftAccumulator );
		const unsigned int rightX = std::ceil( xRightAccumulator );

		const unsigned int tempXY1 = ( (row * width) + leftX );
		const unsigned int tempXY2 = ( (row * width) + rightX );

		const float oneOverPixelStride = 1.0f / ( static_cast<float>( rightX ) - static_cast<float>( leftX ) );
		const float rowF = static_cast<float>( row );
		const float leftXF  = static_cast<float>( leftX );
		const float rightXF = static_cast<float>( rightX );
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

		for ( unsigned int pixel = tempXY1; pixel < tempXY2; pixel += 1 )
		{
			if constexpr ( include3D )
			{
				if ( depthBuffer[pixel] >= depth )
				{
					const float perspOffset = 1.0f / pers;
					const float texCoordX = texX * perspOffset;
					const float texCoordY = texY * perspOffset;
					( *shaderData.fShader )( currentColor, shaderData, 0.0f, 0.0f, 0.0f, texCoordX, texCoordY, light );
					colorProfile.setColor( currentColor );
					if constexpr ( withTransparency )
					{
						colorProfile.template putPixelWithAlphaBlending<width, height>( fb.getPixels(), pixel );
					}
					else
					{
						colorProfile.template putPixel<width, height>( fb.getPixels(), pixel );
						depthBuffer[pixel] = depth;
					}
				}
			}
			else
			{
				const float perspOffset = 1.0f / pers;
				const float texCoordX = texX * perspOffset;
				const float texCoordY = texY * perspOffset;
				( *shaderData.fShader )( currentColor, shaderData, 0.0f, 0.0f, 0.0f, texCoordX, texCoordY, light );
				colorProfile.setColor( currentColor );
				if constexpr ( withTransparency )
				{
					colorProfile.template putPixelWithAlphaBlending<width, height>( fb.getPixels(), pixel );
				}
				else
				{
					colorProfile.template putPixel<width, height>( fb.getPixels(), pixel );
				}
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
template <CP_FORMAT texFormat, bool withTransparency>
inline void SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::renderScanlines (int startRow, int endRowExclusive,
		float x1, float y1, float& xLeftAccumulator, float& xRightAccumulator, float v1PerspMul, float v1Depth, float xLeftIncr,
		float xRightIncr, TriShaderData<texFormat, shaderPassDataSize>& shaderData, Color& currentColor, float texCoordX1,
		float texCoordY1, float texCoordXXIncr, float texCoordXYIncr, float texCoordYXIncr, float texCoordYYIncr, float perspXIncr,
		float perspYIncr, float depthXIncr, float depthYIncr, float v1LightAmnt, float lightAmntXIncr, float lightAmntYIncr,
		std::array<float, width * height>& depthBuffer, int leftHanded)
{
	renderScanlinesHelper<width, height, format, api, include3D, shaderPassDataSize, texFormat, withTransparency>(
			startRow, endRowExclusive, x1, y1, xLeftAccumulator, xRightAccumulator, v1PerspMul, v1Depth, xLeftIncr,
			xRightIncr, shaderData, currentColor, texCoordX1, texCoordY1, texCoordXXIncr, texCoordXYIncr, texCoordYXIncr,
			texCoordYYIncr, perspXIncr, perspYIncr, depthXIncr, depthYIncr, v1LightAmnt, lightAmntXIncr, lightAmntYIncr,
			depthBuffer, m_ColorProfile, m_FB, leftHanded );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <CP_FORMAT texFormat, bool withTransparency>
void SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::renderInBoundsTriangle (Face& face,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData, std::array<float, width * height>& depthBuffer)
{
	// offset to screen space
	face.vertices[0].vec.x() *= (width  - 1);
	face.vertices[0].vec.y() *= (height - 1);
	face.vertices[1].vec.x() *= (width  - 1);
	face.vertices[1].vec.y() *= (height - 1);
	face.vertices[2].vec.x() *= (width  - 1);
	face.vertices[2].vec.y() *= (height - 1);

	triSortVertices( face.vertices[0], face.vertices[1], face.vertices[2], width, height );

	const float x1 = face.vertices[0].vec.x();
	const float y1 = face.vertices[0].vec.y();
	const float x2 = face.vertices[1].vec.x();
	const float y2 = face.vertices[1].vec.y();
	const float x3 = face.vertices[2].vec.x();
	const float y3 = face.vertices[2].vec.y();

	const int y1Ceil = std::ceil( y1 );
	const int y2Ceil = std::ceil( y2 );
	const int y3Ceil = std::ceil( y3 );

	// floats for incrementing xLeftAccumulator and xRightAccumulator
	float xLeftIncrTop     = ( x2 - x1 ) / ( y2 - y1 );
	float xRightIncrTop    = ( x3 - x1 ) / ( y3 - y1 );
	float xLeftIncrBottom  = ( x3 - x2 ) / ( y3 - y2 );
	float xRightIncrBottom = ( x3 - x1 ) / ( y3 - y1 );

	// get handedness by calculating area by getting cross product of vectors of lines 1 and 2
	const float vec1X = x3 - x1;
	const float vec1Y = y3 - y1;
	const float vec2X = x2 - x1;
	const float vec2Y = y2 - y1;

	const float area = ( vec1X * vec2Y ) - ( vec2X * vec1Y );
	const int leftHanded = ( area >= 0 ) ? 1 : 0;

	// depending on the position of the vertices, we need to swap increments
	if ( leftHanded == 0 )
	{
		float tempIncr = xLeftIncrTop;
		xLeftIncrTop = xRightIncrTop;
		xRightIncrTop = tempIncr;

		tempIncr = xLeftIncrBottom;
		xLeftIncrBottom = xRightIncrBottom;
		xRightIncrBottom = tempIncr;
	}

	// floats for x-intercepts (assuming the top of the triangle is pointed for now)
	float xLeftAccumulator  = x1 + ( (static_cast<float>(y1Ceil) - y1) * xLeftIncrTop );
	float xRightAccumulator = x1 + ( (static_cast<float>(y1Ceil) - y1) * xRightIncrTop );

	// get vertex values for gradient calculations
	const float texCoordX1 = face.vertices[0].texCoords.x();
	const float texCoordY1 = face.vertices[0].texCoords.y();
	const float texCoordX2 = face.vertices[1].texCoords.x();
	const float texCoordY2 = face.vertices[1].texCoords.y();
	const float texCoordX3 = face.vertices[2].texCoords.x();
	const float texCoordY3 = face.vertices[2].texCoords.y();
	const float v1PerspMul = 1.0f / face.vertices[0].vec.w();
	const float v2PerspMul = 1.0f / face.vertices[1].vec.w();
	const float v3PerspMul = 1.0f / face.vertices[2].vec.w();
	const float v1Depth = face.vertices[0].vec.z();
	const float v2Depth = face.vertices[1].vec.z();
	const float v3Depth = face.vertices[2].vec.z();
	const Vector<4> lightDir({-0.5f, -0.5f, 0.0f, 0.0f}); // TODO remove this after testing
	const float v1LightAmnt = saturate( face.vertices[0].normal.normalize().dotProduct(lightDir) ) * 0.8f + 0.2f;
	const float v2LightAmnt = saturate( face.vertices[1].normal.normalize().dotProduct(lightDir) ) * 0.8f + 0.2f;
	const float v3LightAmnt = saturate( face.vertices[2].normal.normalize().dotProduct(lightDir) ) * 0.8f + 0.2f;

	// gradient calculation vars
	const float oneOverdX = 1.0f / ( ((x2 - x3) * (y1 - y3)) - ((x1 - x3) * (y2 - y3)) );
	const float oneOverdY = -oneOverdX;
	const Vector<3> texCoordsX({ texCoordX1 * v1PerspMul, texCoordX2 * v2PerspMul, texCoordX3 * v3PerspMul });
	const Vector<3> texCoordsY({ texCoordY1 * v1PerspMul, texCoordY2 * v2PerspMul, texCoordY3 * v3PerspMul });
	const float texCoordXXIncr = calcIncr( texCoordsX, y1, y2, y3, oneOverdX );
	const float texCoordXYIncr = calcIncr( texCoordsX, x1, x2, x3, oneOverdY );
	const float texCoordYXIncr = calcIncr( texCoordsY, y1, y2, y3, oneOverdX );
	const float texCoordYYIncr = calcIncr( texCoordsY, x1, x2, x3, oneOverdY );
	const Vector<3> persps({ v1PerspMul, v2PerspMul, v3PerspMul });
	const float perspXIncr = calcIncr( persps, y1, y2, y3, oneOverdX );
	const float perspYIncr = calcIncr( persps, x1, x2, x3, oneOverdY );
	const Vector<3> depths({ v1Depth, v2Depth, v3Depth });
	const float depthXIncr = calcIncr( depths, y1, y2, y3, oneOverdX );
	const float depthYIncr = calcIncr( depths, x1, x2, x3, oneOverdY );
	const Vector<3> lightAmnts({ v1LightAmnt, v2LightAmnt, v3LightAmnt });
	const float lightAmntXIncr = calcIncr( lightAmnts, y1, y2, y3, oneOverdX );
	const float lightAmntYIncr = calcIncr( lightAmnts, x1, x2, x3, oneOverdY );
	// TODO we actually just want to interpolate the normals, for vertex-shaded lighting we can calculate in the vertex shader and pass down

	Color currentColor;

	// render up until the second vertice
	renderScanlines<texFormat, withTransparency>( y1Ceil, y2Ceil, x1, y1, xLeftAccumulator, xRightAccumulator, v1PerspMul,
		v1Depth, xLeftIncrTop, xRightIncrTop, shaderData, currentColor, texCoordX1, texCoordY1, texCoordXXIncr, texCoordXYIncr,
		texCoordYXIncr, texCoordYYIncr, perspXIncr, perspYIncr, depthXIncr, depthYIncr, v1LightAmnt, lightAmntXIncr, lightAmntYIncr,
		depthBuffer, leftHanded );

	if ( y1Ceil != y2Ceil ) // need to account for vert 2 being in the middle of both left and right increments
	{
		xLeftAccumulator  += ( (static_cast<float>(y2) - y2Ceil) * xLeftIncrTop )  + ( (static_cast<float>(y2Ceil) - y2) * xLeftIncrBottom );
		xRightAccumulator += ( (static_cast<float>(y2) - y2Ceil) * xRightIncrTop ) + ( (static_cast<float>(y2Ceil) - y2) * xRightIncrBottom );
	}
	else if ( ! floatsAreEqual(y1, y2) ) // still need to account for vert 2 being in the middle of both left and right increments
	{
		xLeftAccumulator  = x1 + ( (y2 - y1) * xLeftIncrTop )  + ( (static_cast<float>(y2Ceil) - y2) * xLeftIncrBottom );
		xRightAccumulator = x1 + ( (y2 - y1) * xRightIncrTop ) + ( (static_cast<float>(y2Ceil) - y2) * xRightIncrBottom );
	}
	else
	{
		xLeftAccumulator = x1 + ( (static_cast<float>(y2Ceil) - y2) * xLeftIncrBottom );
		xRightAccumulator = x2 + ( (static_cast<float>(y2Ceil) - y2) * xRightIncrBottom );
	}

	// rasterize up until the last vertice
	renderScanlines<texFormat, withTransparency>( y2Ceil, y3Ceil, x1, y1, xLeftAccumulator, xRightAccumulator, v1PerspMul,
		v1Depth, xLeftIncrBottom, xRightIncrBottom, shaderData, currentColor, texCoordX1, texCoordY1, texCoordXXIncr, texCoordXYIncr,
		texCoordYXIncr, texCoordYYIncr, perspXIncr, perspYIncr, depthXIncr, depthYIncr, v1LightAmnt, lightAmntXIncr, lightAmntYIncr,
		depthBuffer, leftHanded );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <CP_FORMAT texFormat, bool withTransparency>
void SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShadedHelper (Face& face,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData, std::array<float, width * height>& depthBuffer)
{
	// setup shader data
	Camera3D& camera = shaderData.camera;

	// get previous color, since we'll want to set it back when we're done with the shading colors
	const Color previousColor = m_ColorProfile.template getColor<format>();

	// put through the vertex shader first
	( *shaderData.vShader )( shaderData );

	// TODO camera projection should be done in the vertex shader?
	camera.multiplyByCameraMatrix( face );

	// backface culling
	const Vector<4>& vertexVec = face.vertices[0].vec;
	const Vector<4>& normal = face.calcFaceNormals();
	if ( ! (normal.x() * (vertexVec.x() - camera.x())
			+ normal.y() * (vertexVec.y() - camera.y())
			+ normal.z() * (vertexVec.z() - camera.z()) <= 0.0f) )
	{
		return;
	}

	const bool v1Inside = face.vertices[0].isInsideView();
	const bool v2Inside = face.vertices[1].isInsideView();
	const bool v3Inside = face.vertices[2].isInsideView();

	if ( v1Inside && v2Inside && v3Inside )
	{
		camera.perspectiveDivide( face );
		camera.scaleXYToZeroToOne( face );

		// triangle is entirely inside of the clip space, so draw and return
		SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
			renderInBoundsTriangle<texFormat, withTransparency>( face, shaderData, depthBuffer );

		// set the previously used color back since we're done with the gradients
		m_ColorProfile.setColor( previousColor );

		return;
	}

	// otherwise, clip into subtriangles
	constexpr unsigned int maxPossibleVerts = 36; // TODO this is probably excessive
	Vertex outVertices[maxPossibleVerts];
	outVertices[0] = face.vertices[0];
	outVertices[1] = face.vertices[1];
	outVertices[2] = face.vertices[2];
	unsigned int outVerticesSize = 3;

	const float factors[2] = { 1.0f, -1.0f };
	for ( unsigned int factorIndex = 0; factorIndex < 2; factorIndex++ )
	{
		for ( unsigned int axis = 0; axis < 3; axis++ ) // clip x, y, z axis
		{
			Vertex newVertices[maxPossibleVerts];
			unsigned int newVerticesSize = outVerticesSize;
			std::copy( std::begin(outVertices), std::end(outVertices), std::begin(newVertices) );
			outVerticesSize = 0;

			for ( unsigned int vertNum = 0; vertNum < newVerticesSize; vertNum++ )
			{
				Vertex currentVert = newVertices[vertNum];
				Vertex nextVert = newVertices[(vertNum + 1) % newVerticesSize];

				const float currentVertValue = currentVert.vec.at( axis ) * factors[factorIndex];
				const float nextVertValue = nextVert.vec.at( axis ) * factors[factorIndex];

				const bool currentInside = currentVertValue <= currentVert.vec.w();
				const bool nextInside = nextVertValue <= nextVert.vec.w();

				if ( nextInside )
				{
					if ( ! currentInside )
					{
						// add intersection
						const float lerpAmnt = ( currentVert.vec.w() - currentVertValue )
							/ ( (currentVert.vec.w() - currentVertValue) - (nextVert.vec.w() - nextVertValue) );
						Vertex intersect = currentVert.lerp( nextVert, lerpAmnt );
						outVertices[outVerticesSize] = intersect;
						outVerticesSize++;
					}

					outVertices[outVerticesSize] = nextVert;
					outVerticesSize++;
				}
				else if ( currentInside )
				{
					// add intersection
					const float lerpAmnt = ( currentVert.vec.w() - currentVertValue )
						/ ( (currentVert.vec.w() - currentVertValue) - (nextVert.vec.w() - nextVertValue) );
					Vertex intersect = currentVert.lerp( nextVert, lerpAmnt );
					outVertices[outVerticesSize] = intersect;
					outVerticesSize++;
				}
			}
		}
	}

	if ( outVerticesSize > 2 )
	{
		for ( unsigned int vertNum = 1; vertNum < outVerticesSize - 1; vertNum++ )
		{
			Face clippedFace{ outVertices[0], outVertices[vertNum], outVertices[vertNum + 1] };

			camera.perspectiveDivide( clippedFace );
			camera.scaleXYToZeroToOne( clippedFace );

			// triangle is entirely inside of the clip space, so draw
			SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
				renderInBoundsTriangle<texFormat, withTransparency>( clippedFace, shaderData, depthBuffer );
		}
	}

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
	this->drawSpriteHelper<CP_FORMAT::MONOCHROME_1BIT>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::RGBA_32BIT>& sprite)
{
	this->drawSpriteHelper<CP_FORMAT::RGBA_32BIT>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGB_24BIT>& sprite)
{
	this->drawSpriteHelper<CP_FORMAT::RGB_24BIT>( xStart, yStart, sprite );
}

template <CP_FORMAT texFormat, unsigned int shaderPassDataSize>
static inline void basicSpriteVShader (TriShaderData<texFormat, shaderPassDataSize>& vShaderData)
{
}

template <CP_FORMAT texFormat, unsigned int shaderPassDataSize>
static inline void basicSpriteFShader (Color& colorOut, TriShaderData<texFormat, shaderPassDataSize>& fShaderData, float v1Cur, float v2Cur,
		float v3Cur, float texCoordX, float texCoordY, float lightAmnt)
{
	colorOut = fShaderData.textures[0]->getColor( texCoordX, texCoordY );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <CP_FORMAT texFormat>
void SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSpriteHelper (float xStart, float yStart,
		Sprite<texFormat>& sprite)
{
	const float spriteWidthF  = static_cast<float>( sprite.getWidth() );
	const float spriteHeightF = static_cast<float>( sprite.getHeight() );
	const float spriteRotPointXF = static_cast<float>( sprite.getRotationPointX() );
	const float spriteRotPointYF = static_cast<float>( sprite.getRotationPointY() );
	xStart = xStart * static_cast<float>( width );
	yStart = yStart * static_cast<float>( height );

	// create faces
	Face topFace = {{
		{ {{xStart, 			yStart, 		0.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f, 0.0f}}, {{0.0f, 1.0f}} },
		{ {{xStart + spriteWidthF, 	yStart, 		0.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f, 0.0f}}, {{1.0f, 1.0f}} },
		{ {{xStart + spriteWidthF, 	yStart + spriteHeightF, 0.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f, 0.0f}}, {{1.0f, 0.0f}} }
	}};
	Face bottomFace = {{
		{ {{xStart, 			yStart, 		0.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f, 0.0f}}, {{0.0f, 1.0f}} },
		{ {{xStart + spriteWidthF, 	yStart + spriteHeightF, 0.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f, 0.0f}}, {{1.0f, 0.0f}} },
		{ {{xStart, 			yStart + spriteHeightF, 0.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f}} }
	}};

	// create mesh
	Mesh mesh{ {{topFace, bottomFace}} };
	mesh.translate( -xStart - spriteRotPointXF, -yStart - spriteRotPointYF, 0.0f ); // return to rotation point
	mesh.rotate( 0.0f, 0.0f, sprite.getRotationAngle() );
	mesh.applyTransformations();
	mesh.scale( sprite.getScaleFactor() );
	mesh.translate( xStart + spriteRotPointXF, yStart + spriteRotPointYF, 0.0f );

	// TODO allow the ability to use custom shaders in the near future
	std::array<Texture<texFormat>*, 5> textures = { {&sprite.getTexture(), nullptr, nullptr, nullptr, nullptr} };
	Camera3D camera( 0.0f, width, 0.0f, height, 0.0f, 1.0f );
	std::vector<PointLight> lights;
	TriShaderData<texFormat, shaderPassDataSize> shaderData{
		textures,
		camera,
		Color(),
		nullptr,
		basicSpriteVShader,
		basicSpriteFShader
	};

	topFace = mesh.transformedFace( 0 );
	bottomFace = mesh.transformedFace( 1 );

	if constexpr ( include3D )
	{
		SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
			drawTriangleShadedHelper<texFormat, true>( topFace, shaderData,
					SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::m_DepthBuffer );

		SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
			drawTriangleShadedHelper<texFormat, true>( bottomFace, shaderData,
					SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::m_DepthBuffer );
	}
	else
	{
		SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
			drawTriangleShadedHelper<texFormat, true>( topFace, shaderData, *(std::array<float, width * height>*)(nullptr) );

		SoftwareGraphicsBase<width, height, format, api, include3D, shaderPassDataSize>::template
			drawTriangleShadedHelper<texFormat, true>( bottomFace, shaderData, *(std::array<float, width * height>*)(nullptr) );
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawDepthBuffer (Camera3D& camera)
{
	// get previous color, since we'll want to set it back when we're done with the shading colors
	const Color previousColor = m_ColorProfile.template getColor<format>();

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

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void SoftwareGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::clearDepthBuffer()
{
	std::fill( std::begin(m_DepthBuffer), std::end(m_DepthBuffer), std::numeric_limits<float>::max() );
}


#endif // SOFTWAREGRAPHICS_HPP
