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
		std::array<uint8_t, width * height * 3>& getPixels() { return m_Pixels; }
		const unsigned int getNumPixels() const { return m_NumPixels; }
		const float getPixelWidth() const { return m_PixelWidth; }

	protected:
		std::array<uint8_t, width * height * 3>     	m_Pixels;
		const unsigned int 				m_NumPixels = width * height;
		const float 					m_PixelWidth = 3.0f;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferRGBA
{
	public:
		std::array<uint8_t, width * height * 4>& getPixels() { return m_Pixels; }
		const unsigned int getNumPixels() const { return m_NumPixels; }
		const float getPixelWidth() const { return m_PixelWidth; }

	protected:
		std::array<uint8_t, width * height * 4>     	m_Pixels;
		const unsigned int 				m_NumPixels = width * height;
		const float 					m_PixelWidth = 4.0f;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferMonochrome
{
	public:
		std::array<uint8_t, ( width * height ) / 8>& getPixels() { return m_Pixels; }
		const unsigned int getNumPixels() const { return m_NumPixels; }
		const float getPixelWidth() const { return m_PixelWidth; }

	protected:
		std::array<uint8_t, ( width * height ) / 8> 	m_Pixels;
		const unsigned int 				m_NumPixels = ( width * height ) / 8;
		const float 					m_PixelWidth = 1.0f / 8.0f;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBuffer : public std::conditional<format == CP_FORMAT::RGB_24BIT, FrameBufferRGB<width, height, format>,

				typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochrome<width, height, format>,
				FrameBufferRGBA<width, height, format>>::type

				>::type
{
	public:
		FrameBuffer();
		FrameBuffer (uint8_t* pixelData);
		virtual ~FrameBuffer() {}

		unsigned int getWidth() const { return width; }
		unsigned int getHeight() const { return height; }

		const ColorProfile<format> getColorProfile() const { return format; }
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBuffer<width, height, format>::FrameBuffer() :
	std::conditional<format == CP_FORMAT::RGB_24BIT, FrameBufferRGB<width, height, format>,

	typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochrome<width, height, format>,
	FrameBufferRGBA<width, height, format>>::type

	>::type()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBuffer<width, height, format>::FrameBuffer (uint8_t* pixelData)
{
	if constexpr ( format == CP_FORMAT::MONOCHROME_1BIT )
	{
		constexpr unsigned int numBytes = FrameBufferMonochrome<width, height, format>::m_NumPixels
							* FrameBufferMonochrome<width, height, format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferMonochrome<width, height, format>::m_Pixels[byte] = pixelData[byte];
		}
	}
	else if constexpr ( format == CP_FORMAT::RGBA_32BIT )
	{
		constexpr unsigned int numBytes = FrameBufferRGBA<width, height, format>::m_NumPixels
							* FrameBufferRGBA<width, height, format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferRGBA<width, height, format>::m_Pixels[byte] = pixelData[byte];
		}
	}
	else if constexpr ( format == CP_FORMAT::RGB_24BIT )
	{
		constexpr unsigned int numBytes = FrameBufferRGB<width, height, format>::m_NumPixels
							* FrameBufferRGB<width, height, format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferRGB<width, height, format>::m_Pixels[byte] = pixelData[byte];
		}
	}
}

#endif // FRAME_BUFFER_HPP
