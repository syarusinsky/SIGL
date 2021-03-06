#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

/*************************************************************************
 * The Graphics class defines several helper methods to draw graphical
 * elements to a frame buffer. The Graphics constructor is protected (as
 * should be the subclasses that define the implementation), so that
 * only a Surface class can construct them. This is so that typically all
 * drawing code will be done inside a surface. The class is abstract,
 * so that both a software rendering version and a hardware accelerated
 * version can be implemented.
*************************************************************************/

#include "FrameBuffer.hpp"
#include "ColorProfile.hpp"
#include <string>
#include <math.h>

class Font;
class Sprite;

class Graphics
{
	public:
		void setFrameBuffer (FrameBuffer* frameBuffer)
		{
			m_FB = frameBuffer;
			m_ColorProfile = frameBuffer->getColorProfile();
			m_FBWidth = frameBuffer->getWidth();
			m_FBHeight = frameBuffer->getHeight();
			m_FBPixels = frameBuffer->getPixels();
		}

		inline unsigned int convertXPercentageToUInt (float x) { return x * (m_FBWidth  - 1); }
		inline unsigned int convertYPercentageToUInt (float y) { return y * (m_FBHeight - 1); }

		virtual void setColor (float r, float g, float b) = 0;
		virtual void setColor (bool val) = 0;

		virtual void setFont (Font* font) = 0;

		virtual void fill() = 0;
		virtual void drawLine (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawBox (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawTriangleGradient (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawQuadGradient (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawCircle (float originX, float originY, float radius) = 0;
		virtual void drawCircleFilled (float originX, float originY, float radius) = 0;
		virtual void drawText (float xStart, float yStart, const char* text, float scaleFactor) = 0;
		virtual void drawSprite (float xStart, float yStart, Sprite& sprite) = 0;

		inline static float distance (float x1, float y1, float x2, float y2) { return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2)); }

	protected:
		FrameBuffer*         m_FB;
		ColorProfile*        m_ColorProfile;
		Font*                m_CurrentFont;
		unsigned int         m_FBWidth;
		unsigned int         m_FBHeight;
		unsigned int         m_FBNumPixels; 
		uint8_t*             m_FBPixels;

		Graphics (FrameBuffer* frameBuffer) :
			m_FB( frameBuffer ),
			m_ColorProfile( frameBuffer->getColorProfile() ),
			m_FBWidth( frameBuffer->getWidth() ),
			m_FBHeight( frameBuffer->getHeight() ),
			m_FBNumPixels( m_FBWidth * m_FBHeight ),
			m_FBPixels( frameBuffer->getPixels() ) {}
		virtual ~Graphics() {}

};

#endif // GRAPHICS_HPP
