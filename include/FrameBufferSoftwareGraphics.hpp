#ifndef FRAMEBUFFERSOFTWAREGRAPHICS_HPP
#define FRAMEBUFFERSOFTWAREGRAPHICS_HPP

/**************************************************************************
 * The software rendering implementation of the FrameBuffer class.
**************************************************************************/

#include <stdint.h>
#include <array>
#include "ColorProfile.hpp"

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
class FrameBufferSoftwareGraphics : public std::conditional<format == CP_FORMAT::RGB_24BIT, FrameBufferRGB<width, height, format>,

					typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochrome<width, height, format>,
					FrameBufferRGBA<width, height, format>>::type

					>::type
{
	public:
		FrameBufferSoftwareGraphics();
		FrameBufferSoftwareGraphics (uint8_t* pixelData);
		virtual ~FrameBufferSoftwareGraphics() {}

		constexpr unsigned int getWidth() const { return width; }
		constexpr unsigned int getHeight() const { return height; }
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBufferSoftwareGraphics<width, height, format>::FrameBufferSoftwareGraphics() :
	std::conditional<format == CP_FORMAT::RGB_24BIT, FrameBufferRGB<width, height, format>,

	typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochrome<width, height, format>,
	FrameBufferRGBA<width, height, format>>::type

	>::type()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBufferSoftwareGraphics<width, height, format>::FrameBufferSoftwareGraphics (uint8_t* pixelData)
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

#endif // FRAMEBUFFERSOFTWAREGRAPHICS_HPP
