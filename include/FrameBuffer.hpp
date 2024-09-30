#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

/**************************************************************************
 * The FrameBuffer classes define a collection of pixels meant to be a
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
class FrameBufferFixed : public std::conditional<(api == RENDER_API::SOFTWARE), FrameBufferSoftwareGraphicsFixed<width, height, format>,
					FrameBufferOpenGlFixed<width, height, format>>::type
{
	public:
		FrameBufferFixed();
		virtual ~FrameBufferFixed();
};

template <CP_FORMAT format, RENDER_API api>
class FrameBufferDynamic : public std::conditional<(api == RENDER_API::SOFTWARE), FrameBufferSoftwareGraphicsDynamic<format>,
					FrameBufferOpenGlDynamic<format>>::type
{
	public:
		FrameBufferDynamic (unsigned int width, unsigned int height);
		FrameBufferDynamic (unsigned int width, unsigned int height, uint8_t* pixels);
		virtual ~FrameBufferDynamic();
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api>
FrameBufferFixed<width, height, format, api>::FrameBufferFixed() :
	std::conditional<(api == RENDER_API::SOFTWARE), FrameBufferSoftwareGraphicsFixed<width, height, format>,
		FrameBufferOpenGlFixed<width, height, format>>::type()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api>
FrameBufferFixed<width, height, format, api>::~FrameBufferFixed()
{
}

template <CP_FORMAT format, RENDER_API api>
FrameBufferDynamic<format, api>::FrameBufferDynamic (unsigned int width, unsigned int height) :
	std::conditional<(api == RENDER_API::SOFTWARE), FrameBufferSoftwareGraphicsDynamic<format>,
		FrameBufferOpenGlDynamic<format>>( width, height )
{
}

template <CP_FORMAT format, RENDER_API api>
FrameBufferDynamic<format, api>::FrameBufferDynamic (unsigned int width, unsigned int height, uint8_t* pixels) :
	std::conditional<(api == RENDER_API::SOFTWARE), FrameBufferSoftwareGraphicsDynamic<format>,
		FrameBufferOpenGlDynamic<format>>( width, height, pixels )
{
}

template <CP_FORMAT format, RENDER_API api>
FrameBufferDynamic<format, api>::~FrameBufferDynamic()
{
}

#endif // FRAME_BUFFER_HPP
