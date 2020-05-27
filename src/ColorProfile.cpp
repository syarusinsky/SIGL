#include "ColorProfile.hpp"

#include <math.h>

ColorProfile::ColorProfile (const CP_FORMAT& format) :
	m_Format( format ),
	m_RValue( 0 ),
	m_GValue( 0 ),
	m_BValue( 0 ),
	m_MValue( false )
{
}

ColorProfile::~ColorProfile()
{
}

void ColorProfile::putPixel (uint8_t* fbStart, unsigned int pixelNum)
{
	switch ( m_Format )
	{
		case CP_FORMAT::MONOCHROME_1BIT:
		{
			unsigned int byte = std::floor( pixelNum / 8 );
			unsigned int realPixel = pixelNum % 8;
			uint8_t realPixelMask = ( 1 << realPixel );
			if ( m_MValue == true )
			{
				fbStart[byte] = fbStart[byte] | realPixelMask;
			}
			else
			{
				fbStart[byte] = fbStart[byte] & ~(realPixelMask);
			}
		}

			break;
		case CP_FORMAT::RGB_24BIT:
		{
			fbStart[(pixelNum * 3) + 0] = m_RValue; // Red
			fbStart[(pixelNum * 3) + 1] = m_GValue; // Green
			fbStart[(pixelNum * 3) + 2] = m_BValue; // Blue
		}

			break;
		default:
			break;
	}
}

void ColorProfile::setColor (float rValue, float gValue, float bValue)
{
	if ( rValue > 1.0f ) rValue = 1.0f; if ( rValue < 0.0f ) rValue = 0.0f;
	if ( gValue > 1.0f ) gValue = 1.0f; if ( gValue < 0.0f ) gValue = 0.0f;
	if ( bValue > 1.0f ) bValue = 1.0f; if ( bValue < 0.0f ) bValue = 0.0f;

	m_RValue = std::round( 255 * rValue );
	m_GValue = std::round( 255 * gValue );
	m_BValue = std::round( 255 * bValue );

	if ( m_RValue > 0.0f || m_GValue > 0.0f || m_BValue > 0.0f )
	{
		m_MValue = true;
	}
	else
	{
		m_MValue = false;
	}
}

void ColorProfile::setColor (bool mValue)
{
	m_MValue = mValue;
}

CP_FORMAT ColorProfile::getFormat()
{
	return m_Format;
}
