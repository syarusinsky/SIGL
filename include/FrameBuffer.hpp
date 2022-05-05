#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

/**************************************************************************
 * The FrameBuffer class defines a collection of pixels meant to be a
 * representation of the screen, or an off-screen representation of
 * other visual content.
**************************************************************************/

#include <stdint.h>
#include <array>
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

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferRGB
{
	public:
		std::array<uint8_t, width * height * 3>& getPixels() const { return m_Pixels; }

	protected:
		std::array<uint8_t, width * height * 3>     m_Pixels;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferRGBA
{
	public:
		std::array<uint8_t, width * height * 4>& getPixels() const { return m_Pixels; }

	protected:
		std::array<uint8_t, width * height * 4>     m_Pixels;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferMonochrome
{
	public:
		std::array<uint8_t, ( width * height ) / 8>& getPixels() const { return m_Pixels; }

	protected:
		std::array<uint8_t, ( width * height ) / 8> 	m_Pixels;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBuffer : std::conditional<format == CP_FORMAT::RGB_24BIT, FrameBufferRGB<width, height, format>,

				typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochrome<width, height, format>,
				FrameBufferRGBA<width, height, format>>::type

				>::type
{
	public:
		FrameBuffer();
		virtual ~FrameBuffer();

		unsigned int getWidth() const { return width; }
		unsigned int getHeight() const { return height; }

		ColorProfile<format> getColorProfile() { return format; }
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBuffer<width, height, format>::FrameBuffer() :
	std::conditional<format == CP_FORMAT::RGB_24BIT, FrameBufferRGB<width, height, format>,

	typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochrome<width, height, format>,
	FrameBufferRGBA<width, height, format>>::type

	>::type()
{
}

#endif // FRAME_BUFFER_HPP
