#include "ColorProfile.hpp"

#include <math.h>


ColorProfile::ColorProfile (const CP_FORMAT& format) :
	m_Format( format ),
	m_RValue( 0 ),
	m_GValue( 0 ),
	m_BValue( 0 ),
	m_AValue( 255 ),
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
			unsigned int byteNum = std::floor( pixelNum / 8 );
			unsigned int pixelIndex = 7 - (pixelNum % 8);
			uint8_t bitmask = ( 1 << pixelIndex );
			if ( m_MValue == true && m_AValue > 0 )
			{
				fbStart[byteNum] = fbStart[byteNum] | bitmask;
			}
			else if ( m_AValue > 0 )
			{
				fbStart[byteNum] = fbStart[byteNum] & ~(bitmask);
			}
		}

			break;
		case CP_FORMAT::RGB_24BIT:
		{
			// TODO this needs to actually incorporate alpha blending
			if ( m_AValue > 0 )
			{
				fbStart[(pixelNum * 3) + 0] = m_RValue; // Red
				fbStart[(pixelNum * 3) + 1] = m_GValue; // Green
				fbStart[(pixelNum * 3) + 2] = m_BValue; // Blue
			}
		}

			break;
		case CP_FORMAT::RGBA_32BIT:
		{
			// TODO this needs to actually incorporate alpha blending
			if ( m_AValue > 0 )
			{
				fbStart[(pixelNum * 4) + 0] = m_RValue; // Red
				fbStart[(pixelNum * 4) + 1] = m_GValue; // Green
				fbStart[(pixelNum * 4) + 2] = m_BValue; // Blue
				fbStart[(pixelNum * 4) + 3] = m_AValue; // Alpha
			}
		}

			break;
		default:
			break;
	}
}

Color ColorProfile::getPixel (uint8_t* fbStart, unsigned int pixelNum) const
{
	Color color;

	if ( m_Format == CP_FORMAT::MONOCHROME_1BIT )
	{
		color.m_IsMonochrome = true;

		unsigned int byteNum = std::floor( pixelNum / 8 );
		unsigned int pixelIndex = 7 - (pixelNum % 8);
		uint8_t bitmask = ( 1 << pixelIndex );

		uint8_t byte = fbStart[byteNum];

		if ( (byte & bitmask) >> pixelIndex )
		{
			color.m_M = true;
			color.m_R = 1.0f;
			color.m_G = 1.0f;
			color.m_B = 1.0f;
			color.m_A = 1.0f;
		}
		else
		{
			color.m_M = false;
			color.m_R = 0.0f;
			color.m_G = 0.0f;
			color.m_B = 1.0f;
			color.m_A = 0.0f;
		}

		color.m_HasAlpha = false;
	}
	else if ( m_Format == CP_FORMAT::RGB_24BIT )
	{
		color.m_IsMonochrome = false;

		color.m_R = static_cast<float>( fbStart[(pixelNum * 3 ) + 0]) / 255.0f;
		color.m_G = static_cast<float>( fbStart[(pixelNum * 3 ) + 1]) / 255.0f;
		color.m_B = static_cast<float>( fbStart[(pixelNum * 3 ) + 2]) / 255.0f;
		color.m_A = 1.0f;

		if ( color.m_R > 0.0f || color.m_G > 0.0f || color.m_B > 0.0f )
		{
			color.m_M = true;
		}
		else
		{
			color.m_M = false;
		}

		color.m_HasAlpha = false;
	}
	else if ( m_Format == CP_FORMAT::RGBA_32BIT )
	{
		color.m_IsMonochrome = false;

		color.m_R = static_cast<float>( fbStart[(pixelNum * 4 ) + 0]) / 255.0f;
		color.m_G = static_cast<float>( fbStart[(pixelNum * 4 ) + 1]) / 255.0f;
		color.m_B = static_cast<float>( fbStart[(pixelNum * 4 ) + 2]) / 255.0f;
		color.m_A = static_cast<float>( fbStart[(pixelNum * 4 ) + 3]) / 255.0f;

		if ( (color.m_R > 0.0f || color.m_G > 0.0f || color.m_B > 0.0f) && color.m_A > 0.0f )
		{
			color.m_M = true;
		}
		else
		{
			color.m_M = false;
		}

		color.m_HasAlpha = true;
	}

	return color;
}

void ColorProfile::setColor (float rValue, float gValue, float bValue)
{
	if ( rValue > 1.0f ) rValue = 1.0f; if ( rValue < 0.0f ) rValue = 0.0f;
	if ( gValue > 1.0f ) gValue = 1.0f; if ( gValue < 0.0f ) gValue = 0.0f;
	if ( bValue > 1.0f ) bValue = 1.0f; if ( bValue < 0.0f ) bValue = 0.0f;

	m_RValue = std::round( 255 * rValue );
	m_GValue = std::round( 255 * gValue );
	m_BValue = std::round( 255 * bValue );
	m_AValue = 255;

	if ( m_RValue > 0.0f || m_GValue > 0.0f || m_BValue > 0.0f )
	{
		m_MValue = true;
	}
	else
	{
		m_MValue = false;
	}
}

void ColorProfile::setColor (float rValue, float gValue, float bValue, float aValue)
{
	if ( rValue > 1.0f ) rValue = 1.0f; if ( rValue < 0.0f ) rValue = 0.0f;
	if ( gValue > 1.0f ) gValue = 1.0f; if ( gValue < 0.0f ) gValue = 0.0f;
	if ( bValue > 1.0f ) bValue = 1.0f; if ( bValue < 0.0f ) bValue = 0.0f;
	if ( aValue > 1.0f ) aValue = 1.0f; if ( aValue < 0.0f ) aValue = 0.0f;

	m_RValue = std::round( 255 * rValue );
	m_GValue = std::round( 255 * gValue );
	m_BValue = std::round( 255 * bValue );
	m_AValue = std::round( 255 * aValue );

	if ( m_RValue > 0.0f || m_GValue > 0.0f || m_BValue > 0.0f )
	{
		m_MValue = true;
	}
	else
	{
		m_MValue = false;
	}
}

void ColorProfile::setColor (bool mValue, bool useAlpha)
{
	m_MValue = mValue;

	if ( mValue )
	{
		m_RValue = 255;
		m_GValue = 255;
		m_BValue = 255;
		m_AValue = 255;
	}
	else if ( !mValue && useAlpha )
	{
		m_RValue = 0;
		m_GValue = 0;
		m_BValue = 0;
		m_AValue = 0;
	}
	else
	{
		m_RValue = 0;
		m_GValue = 0;
		m_BValue = 0;
		m_AValue = 255;
	}
}

void ColorProfile::setColor (const Color& color)
{
	m_RValue = std::round( 255 * color.m_R );
	m_GValue = std::round( 255 * color.m_G );
	m_BValue = std::round( 255 * color.m_B );
	m_AValue = std::round( 255 * color.m_A );
	m_MValue = color.m_M;
}

const CP_FORMAT ColorProfile::getFormat() const
{
	return m_Format;
}
