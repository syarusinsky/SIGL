#include "Sprite.hpp"

#include <math.h>

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

Sprite::Sprite (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	FrameBuffer( width, height, format ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0 ),
	m_RotPointX( m_Width / 2 ),
	m_RotPointY( m_Height / 2 )
{
}

Sprite::Sprite (uint8_t* data) :
	FrameBuffer( (data[1] << 3) | (data[2] << 2) | (data[3] << 1) | data[4],
			(data[5] << 3) | (data[6] << 2) | (data[7] << 1) | data[8],
			FormatInitializer( data[0] ).getFormat(),
			&data[9] ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0 ),
	m_RotPointX( m_Width / 2 ),
	m_RotPointY( m_Height / 2 )
{
}

unsigned int Sprite::getScaledWidth() const
{
	return std::floor( m_Width * m_ScaleFactor );
}

unsigned int Sprite::getScaledHeight() const
{
	return std::floor( m_Height * m_ScaleFactor );
}

void Sprite::setScaleFactor (float scaleFactor)
{
	m_ScaleFactor = scaleFactor;
}

float Sprite::getScaleFactor() const
{
	return m_ScaleFactor;
}

void Sprite::setRotationAngle (int degrees)
{
	if ( degrees < 0 )
	{
		degrees = std::abs( degrees ) % 360;
		m_RotationDegrees = 360 - degrees;
	}
	else
	{
		m_RotationDegrees = degrees % 360;
	}
}

int Sprite::getRotationAngle() const
{
	return m_RotationDegrees;
}

void Sprite::setRotationPointX (float x)
{
	m_RotPointX = ( m_Width - 1 ) * x;
}

int Sprite::getRotationPointX() const
{
	return m_RotPointX;
}

void Sprite::setRotationPointY (float y)
{
	m_RotPointY = ( m_Height - 1 ) * y;
}

int Sprite::getRotationPointY() const
{
	return m_RotPointY;
}
