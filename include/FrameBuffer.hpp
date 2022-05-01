#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

/**************************************************************************
 * The FrameBuffer class defines a collection of pixels meant to be a
 * representation of the screen, or an off-screen representation of
 * other visual content.
**************************************************************************/

#include <stdint.h>
#include "ColorProfile.hpp"

class FormatInitializer
{
	public:
		FormatInitializer (uint8_t format)
		{
			if ( format == 0 )
			{
				m_Format = CP_FORMAT::RGB_24BIT;
			}
			else if ( format == 1 )
			{
				m_Format = CP_FORMAT::RGBA_32BIT;
			}
			else if ( format == 2 )
			{
				m_Format = CP_FORMAT::MONOCHROME_1BIT;
			}
			else
			{
				m_Format = CP_FORMAT::RGB_24BIT;
			}
		}

		CP_FORMAT getFormat() { return m_Format; }

	private:
		CP_FORMAT m_Format;
};

class FrameBuffer
{
	public:
		FrameBuffer (unsigned int width, unsigned int height, const CP_FORMAT& format);
		FrameBuffer (unsigned int width, unsigned int height, const CP_FORMAT& format, uint8_t* pixels);
		virtual ~FrameBuffer();

		uint8_t* getPixels() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;

		ColorProfile* getColorProfile();

	protected:
		ColorProfile m_ColorProfile;
		uint8_t*     m_Pixels;

		unsigned int m_Width;
		unsigned int m_Height;
};

#endif // FRAME_BUFFER_HPP
