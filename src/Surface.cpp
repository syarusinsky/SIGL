#include "Surface.hpp"

#include "FrameBuffer.hpp"

#ifdef SOFTWARE_RENDERING
#include "SoftwareGraphics.hpp"
#else
#include "SoftwareGraphics.hpp" // TODO if I ever implement hardware acceleration, this should be changed
#endif // SOFTWARE_RENDERING

Surface::Surface (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	m_FrameBuffer( new FrameBuffer(width, height, format) ),
	m_ColorProfile( m_FrameBuffer->getColorProfile() ),
#ifdef SOFTWARE_RENDERING
	m_Graphics( new SoftwareGraphics(m_FrameBuffer) )
#else
	m_Graphics( new SoftwareGraphics(m_FrameBuffer) ) // TODO if I ever implement hardware acceleration, this should be changed
#endif // SOFTWARE_RENDERING
{
}

Surface::~Surface()
{
}

FrameBuffer* Surface::getFrameBuffer()
{
	return m_FrameBuffer;
}

ColorProfile* Surface::getColorProfile()
{
	return m_ColorProfile;
}

unsigned int Surface::getPixelWidthInBits() const
{
	switch ( m_ColorProfile->getFormat() )
	{
		case CP_FORMAT::RGB_24BIT:
			return 24;
		case CP_FORMAT::MONOCHROME_1BIT:
			return 1;
		default:
			return 0;
	}
}
