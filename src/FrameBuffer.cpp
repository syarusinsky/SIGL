#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, const CP_FORMAT& format) :
	m_ColorProfile( format ),
	m_Pixels( nullptr ),
	m_Width( width ),
	m_Height( height )
{
	switch ( format )
	{
		case CP_FORMAT::RGB_24BIT:
			m_Pixels = new uint8_t[width * height * 3];

			break;
		case CP_FORMAT::MONOCHROME_1BIT:
			m_Pixels = new uint8_t[(width * height) / 8];

			break;
		default:
			break;
	}
}

FrameBuffer::~FrameBuffer()
{
}

uint8_t* FrameBuffer::getPixels()
{
	return m_Pixels;
}

unsigned int FrameBuffer::getWidth()
{
	return m_Width;
}

unsigned int FrameBuffer::getHeight()
{
	return m_Height;
}

ColorProfile* FrameBuffer::getColorProfile()
{
	return &m_ColorProfile;
}
