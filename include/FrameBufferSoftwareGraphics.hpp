#ifndef FRAMEBUFFERSOFTWAREGRAPHICS_HPP
#define FRAMEBUFFERSOFTWAREGRAPHICS_HPP

/**************************************************************************
 * The software rendering implementation of the FrameBuffer class.
**************************************************************************/

#include <stdint.h>
#include <array>
#include <vector>
#include "ColorProfile.hpp"

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferRGBFixed
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

template <CP_FORMAT format>
class FrameBufferRGBDynamic
{
	public:
		FrameBufferRGBDynamic (unsigned int width, unsigned int height) :
			m_Pixels( std::vector<uint8_t>(width * height * 3, 0) ),
			m_NumPixels( width * height ) {}
		std::vector<uint8_t>& getPixels() { return m_Pixels; }
		const unsigned int getNumPixels() const { return m_NumPixels; }
		const float getPixelWidth() const { return m_PixelWidth; }

	protected:
		std::vector<uint8_t>     			m_Pixels;
		const unsigned int 				m_NumPixels;
		const float 					m_PixelWidth = 3.0f;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferRGBAFixed
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

template <CP_FORMAT format>
class FrameBufferRGBADynamic
{
	public:
		FrameBufferRGBADynamic (unsigned int width, unsigned int height) :
			m_Pixels( std::vector<uint8_t>(width * height * 4, 0) ),
			m_NumPixels( width * height ) {}
		std::vector<uint8_t>& getPixels() { return m_Pixels; }
		const unsigned int getNumPixels() const { return m_NumPixels; }
		const float getPixelWidth() const { return m_PixelWidth; }

	protected:
		std::vector<uint8_t>     			m_Pixels;
		const unsigned int 				m_NumPixels;
		const float 					m_PixelWidth = 4.0f;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferMonochromeFixed
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

template <CP_FORMAT format>
class FrameBufferMonochromeDynamic
{
	public:
		FrameBufferMonochromeDynamic (unsigned int width, unsigned int height) :
			m_Pixels( std::vector<uint8_t>((width * height) / 8, 0) ),
			m_NumPixels( (width * height) / 8 ) {}
		std::vector<uint8_t>& getPixels() { return m_Pixels; }
		const unsigned int getNumPixels() const { return m_NumPixels; }
		const float getPixelWidth() const { return m_PixelWidth; }

	protected:
		std::vector<uint8_t> 				m_Pixels;
		const unsigned int 				m_NumPixels;
		const float 					m_PixelWidth = 1.0f / 8.0f;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferSoftwareGraphicsFixed : public std::conditional<format == CP_FORMAT::RGB_24BIT || format == CP_FORMAT::BGR_24BIT, FrameBufferRGBFixed<width, height, format>,

					typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochromeFixed<width, height, format>,
					FrameBufferRGBAFixed<width, height, format>>::type

					>::type
{
	public:
		FrameBufferSoftwareGraphicsFixed();
		FrameBufferSoftwareGraphicsFixed (uint8_t* pixelData);
		virtual ~FrameBufferSoftwareGraphicsFixed() {}

		constexpr unsigned int getWidth() const { return width; }
		constexpr unsigned int getHeight() const { return height; }
};

template <CP_FORMAT format>
class FrameBufferSoftwareGraphicsDynamic : public std::conditional<format == CP_FORMAT::RGB_24BIT || format == CP_FORMAT::BGR_24BIT, FrameBufferRGBDynamic<format>,

					typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochromeDynamic<format>,
					FrameBufferRGBADynamic<format>>::type

					>::type
{
	public:
		FrameBufferSoftwareGraphicsDynamic (unsigned int width, unsigned int height);
		FrameBufferSoftwareGraphicsDynamic (unsigned int width, unsigned int height, uint8_t* pixelData);
		virtual ~FrameBufferSoftwareGraphicsDynamic() {}

		Color getColor (unsigned int width, unsigned int height) const;
		Color getColor (float x, float y) const;

		unsigned int getWidth() const { return m_Width; }
		unsigned int getHeight() const { return m_Height; }

	protected:
		ColorProfile<format> m_ColorProfile;
		const unsigned int m_Width;
		const unsigned int m_Height;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBufferSoftwareGraphicsFixed<width, height, format>::FrameBufferSoftwareGraphicsFixed()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBufferSoftwareGraphicsFixed<width, height, format>::FrameBufferSoftwareGraphicsFixed (uint8_t* pixelData)
{
	if constexpr ( format == CP_FORMAT::MONOCHROME_1BIT )
	{
		constexpr unsigned int numBytes = FrameBufferMonochromeFixed<width, height, format>::m_NumPixels
							* FrameBufferMonochromeFixed<width, height, format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferMonochromeFixed<width, height, format>::m_Pixels[byte] = pixelData[byte];
		}
	}
	else if constexpr ( format == CP_FORMAT::RGBA_32BIT )
	{
		constexpr unsigned int numBytes = FrameBufferRGBAFixed<width, height, format>::m_NumPixels
							* FrameBufferRGBAFixed<width, height, format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferRGBAFixed<width, height, format>::m_Pixels[byte] = pixelData[byte];
		}
	}
	else if constexpr ( format == CP_FORMAT::RGB_24BIT || format == CP_FORMAT::BGR_24BIT )
	{
		constexpr unsigned int numBytes = FrameBufferRGBFixed<width, height, format>::m_NumPixels
							* FrameBufferRGBFixed<width, height, format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferRGBFixed<width, height, format>::m_Pixels[byte] = pixelData[byte];
		}
	}
}

template <CP_FORMAT format>
FrameBufferSoftwareGraphicsDynamic<format>::FrameBufferSoftwareGraphicsDynamic (unsigned int width, unsigned int height) :
std::conditional<format == CP_FORMAT::RGB_24BIT || format == CP_FORMAT::BGR_24BIT, FrameBufferRGBDynamic<format>,

	typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochromeDynamic<format>,
	FrameBufferRGBADynamic<format>>::type

	>::type( width, height ),
	m_Width( width ),
	m_Height( height )
{
}

template <CP_FORMAT format>
FrameBufferSoftwareGraphicsDynamic<format>::FrameBufferSoftwareGraphicsDynamic (unsigned int width, unsigned int height, uint8_t* pixelData) :
std::conditional<format == CP_FORMAT::RGB_24BIT || format == CP_FORMAT::BGR_24BIT, FrameBufferRGBDynamic<format>,

	typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, FrameBufferMonochromeDynamic<format>,
	FrameBufferRGBADynamic<format>>::type

	>::type( width, height ),
	m_Width( width ),
	m_Height( height )
{
	if constexpr ( format == CP_FORMAT::MONOCHROME_1BIT )
	{
		const unsigned int numBytes = FrameBufferMonochromeDynamic<format>::m_NumPixels
							* FrameBufferMonochromeDynamic<format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferMonochromeDynamic<format>::m_Pixels[byte] = pixelData[byte];
		}
	}
	else if constexpr ( format == CP_FORMAT::RGBA_32BIT )
	{
		const unsigned int numBytes = FrameBufferRGBADynamic<format>::m_NumPixels
							* FrameBufferRGBADynamic<format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferRGBADynamic<format>::m_Pixels[byte] = pixelData[byte];
		}
	}
	else if constexpr ( format == CP_FORMAT::RGB_24BIT || format == CP_FORMAT::BGR_24BIT )
	{
		const unsigned int numBytes = FrameBufferRGBDynamic<format>::m_NumPixels
							* FrameBufferRGBDynamic<format>::m_PixelWidth;
		for ( unsigned int byte = 0; byte < numBytes; byte++ )
		{
			FrameBufferRGBDynamic<format>::m_Pixels[byte] = pixelData[byte];
		}
	}
}

template <CP_FORMAT format>
Color FrameBufferSoftwareGraphicsDynamic<format>::getColor (unsigned int x, unsigned int y) const
{
	const unsigned int pixelNum = ( m_Width * y ) + x;

	if constexpr ( format == CP_FORMAT::MONOCHROME_1BIT )
	{
		return m_ColorProfile.getPixel(
				static_cast<const uint8_t*>(&FrameBufferMonochromeDynamic<format>::m_Pixels[0]),
				pixelNum );
	}
	else if constexpr ( format == CP_FORMAT::RGBA_32BIT )
	{
		return m_ColorProfile.getPixel(
				static_cast<const uint8_t*>(&FrameBufferRGBADynamic<format>::m_Pixels[0]),
				pixelNum );
	}
	else if constexpr ( format == CP_FORMAT::RGB_24BIT || format == CP_FORMAT::BGR_24BIT )
	{
		return m_ColorProfile.getPixel(
				static_cast<const uint8_t*>(&FrameBufferRGBDynamic<format>::m_Pixels[0]),
				pixelNum );
	}
}

template <CP_FORMAT format>
Color FrameBufferSoftwareGraphicsDynamic<format>::getColor (float x, float y) const
{
	unsigned int xInt = ( x * (m_Width  - 1) );
	unsigned int yInt = ( y * (m_Height - 1) );

	return this->getColor( xInt, yInt );
}

#endif // FRAMEBUFFERSOFTWAREGRAPHICS_HPP
