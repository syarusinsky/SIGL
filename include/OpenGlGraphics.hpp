#ifndef OPENGLGRAPHICS_HPP
#define OPENGLGRAPHICS_HPP

/**************************************************************************
 * The OpenGlGraphics class defines functions used to render graphics
 * with the OpenGL api rendering api.
**************************************************************************/

#include "Graphics.hpp"

// just so code isn't insanely wide
#define m_CP Graphics<width, height, format, include3D, shaderPassDataSize>::m_ColorProfile
#define m_CurrentFont Graphics<width, height, format, include3D, shaderPassDataSize>::m_CurrentFont
#define m_Pxls Graphics<width, height, format, include3D, shaderPassDataSize>::m_FB.getPixels()
#define m_DepthBuffer Graphics3D<width, height, shaderPassDataSize>::m_DepthBuffer
#define m_ShaderPassData Graphics3D<width, height, shaderPassDataSize>::m_ShaderPassData
#define m_NumPxls Graphics<width, height, format, include3D, shaderPassDataSize>::m_FB.getNumPixels()

#include "Font.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"
#include "Engine3D.hpp"

#include <algorithm>
#include <limits>

// just to avoid compilation error
template <unsigned int width, unsigned int height, CP_FORMAT format>
class OpenGlGraphicsNo3D : public Graphics<width, height, format, false, 0>
{
};

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
class OpenGlGraphics3D : public Graphics<width, height, format, true, shaderPassDataSize>
{
	public:
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData) override;
		void drawDepthBuffer (Camera3D& camera) override;

	protected:
		template <CP_FORMAT texFormat> void drawTriangleShadedHelper (Face& face, TriShaderData<texFormat, shaderPassDataSize>& shaderData);
};

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
class OpenGlGraphics 	: public std::conditional<include3D, OpenGlGraphics3D<width, height, format, include3D, shaderPassDataSize>,
								OpenGlGraphicsNo3D<width, height, format>>::type
{
	// only a surface should be able to construct
	template<unsigned int w, unsigned int h, CP_FORMAT f, unsigned int nT, bool i3D, unsigned int sPDS> friend class SurfaceThreaded;
	template<unsigned int w, unsigned int h, CP_FORMAT f, bool i3D, unsigned int sPDS> friend class SurfaceSingleCore;

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

	protected:
		template <typename S>
		void drawSpriteHelper (float xStart, float yStart, S& sprite);

		OpenGlGraphics();
		~OpenGlGraphics() override;
};

inline bool floatsAreEqual (float x, float y, float diff = std::numeric_limits<float>::epsilon() )
{
	if ( fabs(x - y) < diff ) return true;

	return false;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::OpenGlGraphics()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::~OpenGlGraphics()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::setColor (float r, float g, float b)
{
	m_CP.setColor( r, g, b );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::setColor (bool val)
{
	m_CP.setColor( val );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::setFont (Font* font)
{
	m_CurrentFont = font;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::fill()
{
	// TODO get the color from the color profile, fill the framebuffer
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawLine (float xStart, float yStart, float xEnd, float yEnd)
{
	// clip line and return if line is off screen
	if ( !Graphics<width, height, format, include3D, shaderPassDataSize>::clipLine( &xStart, &yStart, &xEnd, &yEnd ) ) return;

	// TODO get the color from the color profile, draw the line
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawBox (float xStart, float yStart, float xEnd, float yEnd)
{
	drawLine( xStart, yStart, xEnd,   yStart );
	drawLine( xEnd,   yStart, xEnd,   yEnd   );
	drawLine( xEnd,   yEnd,   xStart, yEnd   );
	drawLine( xStart, yEnd,   xStart, yStart );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd)
{
	// TODO clip the box from 0.0 to 1.0, get color from color profile, draw filled box
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x1, y1 );
}

static inline void triSortVertices (Vertex& v1, Vertex& v2, Vertex& v3, float width, float height)
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

static inline void triSortVertices (int& x1Sorted, int& y1Sorted, float& x1FSorted, float& y1FSorted,
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

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3)
{
	// TODO possibly clip, get color from color profile, then draw filled triangle
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::MONOCHROME_1BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGBA_32BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGB_24BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
template <CP_FORMAT texFormat>
void OpenGlGraphics3D<width, height, format, include3D, shaderPassDataSize>::drawTriangleShadedHelper (Face& face,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData)
{
	// TODO draw shaded triangle using shaderData
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawQuad (float x1, float y1, float x2, float y2, float x3, float y3,
		float x4, float y4)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x4, y4 );
	drawLine( x4, y4, x1, y1 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3,
		float x4, float y4)
{
	drawTriangleFilled( x1, y1, x2, y2, x3, y3 );
	drawTriangleFilled( x1, y1, x4, y4, x3, y3 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawCircle (float originX, float originY, float radius)
{
	// TODO get color from color profile, draw circle
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawCircleFilled (float originX, float originY, float radius)
{
	// TODO get color from color profile, draw filled circle
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawText (float xStart, float yStart, const char* text, float scaleFactor)
{
	// TODO get color from color profile, draw text
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::MONOCHROME_1BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::MONOCHROME_1BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGBA_32BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGBA_32BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGB_24BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGB_24BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
template <typename S>
void OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>::drawSpriteHelper (float xStart, float yStart, S& sprite)
{
	// TODO draw sprite
}

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, include3D, shaderPassDataSize>::drawDepthBuffer (Camera3D& camera)
{
	// TODO render depth buffer
}

#undef m_CP
#undef m_CurrentFont
#undef m_Pxls
#undef m_DepthBuffer
#undef m_ShaderPassData
#undef m_NumPxls

#endif // OPENGLGRAPHICS_HPP
