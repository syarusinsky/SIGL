#ifndef OPENGLGRAPHICS_HPP
#define OPENGLGRAPHICS_HPP

/**************************************************************************
 * The OpenGlGraphics class defines functions used to render graphics
 * with the OpenGL api rendering api.
**************************************************************************/

#include "IGraphics.hpp"

// just so code isn't insanely wide
#define m_CP IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_ColorProfile
#define m_CurrentFont IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_CurrentFont
#define m_DepthBuffer IGraphics3D<width, height, shaderPassDataSize>::m_DepthBuffer
#define m_ShaderPassData IGraphics3D<width, height, shaderPassDataSize>::m_ShaderPassData

#include "Font.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"
#include "Engine3D.hpp"

#include <algorithm>
#include <limits>

// just to avoid compilation error
template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api>
class OpenGlGraphicsNo3D : public IGraphics<width, height, format, api, false, 0>
{
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class OpenGlGraphics3D : public IGraphics<width, height, format, api, true, shaderPassDataSize>
{
	public:
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData) override;
		void drawDepthBuffer (Camera3D& camera) override;

	protected:
		template <CP_FORMAT texFormat> void drawTriangleShadedHelper (Face& face, TriShaderData<texFormat, shaderPassDataSize>& shaderData);
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class OpenGlGraphics 	: public std::conditional<include3D, OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>,
								OpenGlGraphicsNo3D<width, height, format, api>>::type
{
	// only a surface should be able to construct
	template<RENDER_API rAPI, unsigned int w, unsigned int h, CP_FORMAT f, unsigned int nT, bool i3D, unsigned int sPDS> friend class SurfaceThreaded;
	template<RENDER_API rAPI, unsigned int w, unsigned int h, CP_FORMAT f, bool i3D, unsigned int sPDS> friend class SurfaceSingleCore;

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
		virtual ~OpenGlGraphics() override;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::OpenGlGraphics()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::~OpenGlGraphics()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::setColor (float r, float g, float b)
{
	m_CP.setColor( r, g, b );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::setColor (bool val)
{
	m_CP.setColor( val );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::setFont (Font* font)
{
	m_CurrentFont = font;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::fill()
{
	// TODO get the color from the color profile, fill the framebuffer
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawLine (float xStart, float yStart, float xEnd, float yEnd)
{
	// clip line and return if line is off screen
	if ( !IGraphics<width, height, format, api, include3D, shaderPassDataSize>::clipLine( &xStart, &yStart, &xEnd, &yEnd ) ) return;

	// TODO get the color from the color profile, draw the line
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawBox (float xStart, float yStart, float xEnd, float yEnd)
{
	drawLine( xStart, yStart, xEnd,   yStart );
	drawLine( xEnd,   yStart, xEnd,   yEnd   );
	drawLine( xEnd,   yEnd,   xStart, yEnd   );
	drawLine( xStart, yEnd,   xStart, yStart );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd)
{
	// TODO clip the box from 0.0 to 1.0, get color from color profile, draw filled box
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x1, y1 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleFilled (float x1, float y1, float x2, float y2, float x3,
		float y3)
{
	// TODO possibly clip, get color from color profile, then draw filled triangle
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::MONOCHROME_1BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGBA_32BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT,
		shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGB_24BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <CP_FORMAT texFormat>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShadedHelper (Face& face,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData)
{
	// TODO draw shaded triangle using shaderData
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawQuad (float x1, float y1, float x2, float y2, float x3, float y3,
		float x4, float y4)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x4, y4 );
	drawLine( x4, y4, x1, y1 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3,
		float x4, float y4)
{
	drawTriangleFilled( x1, y1, x2, y2, x3, y3 );
	drawTriangleFilled( x1, y1, x4, y4, x3, y3 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircle (float originX, float originY, float radius)
{
	// TODO get color from color profile, draw circle
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircleFilled (float originX, float originY, float radius)
{
	// TODO get color from color profile, draw filled circle
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawText (float xStart, float yStart, const char* text, float scaleFactor)
{
	// TODO get color from color profile, draw text
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::MONOCHROME_1BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::MONOCHROME_1BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::RGBA_32BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGBA_32BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::RGB_24BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGB_24BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <typename S>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSpriteHelper (float xStart, float yStart, S& sprite)
{
	// TODO draw sprite
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawDepthBuffer (Camera3D& camera)
{
	// TODO render depth buffer
}

#undef m_CP
#undef m_CurrentFont
#undef m_DepthBuffer
#undef m_ShaderPassData

#endif // OPENGLGRAPHICS_HPP
