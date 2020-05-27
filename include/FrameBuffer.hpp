#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

/**************************************************************************
 * The FrameBuffer class defines a collection of pixels meant to be a
 * representation of the screen, or an off-screen representation of
 * other visual content.
**************************************************************************/

#include <stdint.h>
#include "ColorProfile.hpp"

class FrameBuffer
{
	public:
		FrameBuffer (unsigned int width, unsigned int height, const CP_FORMAT& format);
		virtual ~FrameBuffer();

		uint8_t*     getPixels();
		unsigned int getWidth();
		unsigned int getHeight();

		ColorProfile* getColorProfile();

	private:
		ColorProfile m_ColorProfile;
		uint8_t*     m_Pixels;

		unsigned int m_Width;
		unsigned int m_Height;
};

#endif // FRAME_BUFFER_HPP
