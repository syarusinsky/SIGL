#include "Surface.hpp"

Surface::Surface (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	m_FrameBuffer( width, height, format ),
	m_ColorProfile( m_FrameBuffer.getColorProfile() ),
	m_Graphics( &m_FrameBuffer, m_ColorProfile )
{
}

Surface::~Surface()
{
}

FrameBuffer* Surface::getFrameBuffer()
{
	return &m_FrameBuffer;
}

ColorProfile* Surface::getColorProfile()
{
	return m_ColorProfile;
}

unsigned int Surface::getPixelWidthInBits()
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
