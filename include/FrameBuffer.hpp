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

#include "FrameBufferSoftwareGraphics.hpp"
#include "FrameBufferOpenGl.hpp"

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

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api>
class FrameBufferFixed : public std::conditional<(api == RENDER_API::SOFTWARE), FrameBufferSoftwareGraphics<width, height, format>,
					FrameBufferOpenGl<width, height, format>>::type
{
	public:
		FrameBufferFixed();
		virtual ~FrameBufferFixed();
};

template <CP_FORMAT format>
class FrameBufferDynamic
{
	public:
		FrameBufferDynamic (unsigned int width, unsigned int height);
		FrameBufferDynamic (unsigned int width, unsigned int height, uint8_t* pixels);
		virtual ~FrameBufferDynamic();

		Color getColor (unsigned int x, unsigned int y);
		Color getColor (float x, float y);

		unsigned int getWidth() const;
		unsigned int getHeight() const;

		uint8_t* getData() { return m_Pixels; }

	protected:
		ColorProfile<format> m_ColorProfile;
		uint8_t*             m_Pixels;

		const unsigned int m_Width;
		const unsigned int m_Height;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api>
FrameBufferFixed<width, height, format, api>::FrameBufferFixed() :
	std::conditional<(api == RENDER_API::SOFTWARE), FrameBufferSoftwareGraphics<width, height, format>,
		FrameBufferOpenGl<width, height, format>>::type()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api>
FrameBufferFixed<width, height, format, api>::~FrameBufferFixed()
{
}

template <CP_FORMAT format>
FrameBufferDynamic<format>::FrameBufferDynamic (unsigned int width, unsigned int height) :
	m_ColorProfile(),
	m_Pixels( nullptr ),
	m_Width( width ),
	m_Height( height )
{
	if constexpr ( format == CP_FORMAT::RGB_24BIT )
	{
		m_Pixels = new uint8_t[width * height * 3];
	}
	else if constexpr ( format == CP_FORMAT::RGBA_32BIT )
	{
		m_Pixels = new uint8_t[width * height * 4];
	}
	else if constexpr ( format == CP_FORMAT::MONOCHROME_1BIT )
	{
		m_Pixels = new uint8_t[(width * height) / 8];
	}
}

template <CP_FORMAT format>
FrameBufferDynamic<format>::FrameBufferDynamic (unsigned int width, unsigned int height, uint8_t* pixels) :
	m_ColorProfile(),
	m_Pixels( pixels ),
	m_Width( width ),
	m_Height( height )
{
}

template <CP_FORMAT format>
FrameBufferDynamic<format>::~FrameBufferDynamic()
{
}

template <CP_FORMAT format>
Color FrameBufferDynamic<format>::getColor (unsigned int x, unsigned y)
{
	const unsigned int pixelNum = ( m_Width * y ) + x;
	return m_ColorProfile.getPixel( m_Pixels, pixelNum );
}

template <CP_FORMAT format>
Color FrameBufferDynamic<format>::getColor (float x, float y)
{
	unsigned int xInt = ( x * (m_Width  - 1) );
	unsigned int yInt = ( y * (m_Height - 1) );

	return this->getColor( xInt, yInt );
}

template <CP_FORMAT format>
unsigned int FrameBufferDynamic<format>::getWidth() const
{
	return m_Width;
}

template <CP_FORMAT format>
unsigned int FrameBufferDynamic<format>::getHeight() const
{
	return m_Height;
}

#endif // FRAME_BUFFER_HPP
