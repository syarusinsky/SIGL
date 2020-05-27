#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

/*************************************************************************
 * The Graphics class defines several helper methods to draw graphical
 * elements to a frame buffer. Typically, a Graphics object will be
 * instantiated with a reference to a FrameBuffer which will be the
 * active context to paint within until it is changed by the user. The
 * class is pure virtual, so that both a software rendering version and
 * a hardware accelerated version can be implemented.
*************************************************************************/

#include "FrameBuffer.hpp"
#include "ColorProfile.hpp"
#include "Font.hpp"
#include <string>

class Graphics
{
	public:
		Graphics (FrameBuffer* frameBuffer, ColorProfile* colorProfile) :
			m_FB (frameBuffer),
			m_ColorProfile (colorProfile),
			m_FBWidth (frameBuffer->getWidth()),
			m_FBHeight (frameBuffer->getHeight()),
			m_FBPixels (frameBuffer->getPixels()) {}
		virtual ~Graphics() {}

		virtual void setColor (float r, float g, float b) = 0; // TODO change these to also support monochrome

		virtual void setFont (Font* font) = 0;

		virtual void fill() = 0;
		virtual void drawLine (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawBox (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawCircle (float originX, float originY, float radius) = 0;
		virtual void drawCircleFilled (float originX, float originY, float radius) = 0;
		virtual void drawText (float xStart, float yStart, std::string, float scaleFactor) = 0;

	protected:
		FrameBuffer*       m_FB;
		ColorProfile*      m_ColorProfile;
		Font*              m_CurrentFont;
		const unsigned int m_FBWidth;
		const unsigned int m_FBHeight;
		uint8_t* const     m_FBPixels;
};

#endif // GRAPHICS_HPP
