#ifndef SPRITE_HPP
#define SPRITE_HPP

/**************************************************************************
 * The Sprite class defines a framebuffer that also contains scaling
 * and rotation information. A Sprite can be 'blitted' onto another
 * FrameBuffer and will also be scaled/rotated according to the sprites
 * current scaling and rotation values.
**************************************************************************/

#include "FrameBuffer.hpp"

#include <math.h>

template <unsigned int width, unsigned int height, CP_FORMAT format>
class Sprite : public FrameBuffer<width, height, format>
{
	public:
		Sprite();
		Sprite (uint8_t* data);

		unsigned int getScaledWidth() const;
		unsigned int getScaledHeight() const;

		void setScaleFactor (float scaleFactor);
		float getScaleFactor() const;

		void setRotationAngle (int degrees);
		int getRotationAngle() const;
		void setRotationPointX (float x); // should be between 0.0f and 1.0f
		int getRotationPointX() const;
		void setRotationPointY (float y); // should be between 0.0f and 1.0f
		int getRotationPointY() const;

	protected:
		float m_ScaleFactor;
		int   m_RotationDegrees;
		int   m_RotPointX;
		int   m_RotPointY;

	private:
		ColorProfile<format> m_ColorProfile;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
Sprite<width, height, format>::Sprite() :
	FrameBuffer<width, height, format>(),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0 ),
	m_RotPointX( width / 2 ),
	m_RotPointY( height / 2 )
{
}

// for sif files,...
// width: (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4]
// height: (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]
// format: FormatInitializer( data[0] ).getFormat()
// pixels: &data[9]
template <unsigned int width, unsigned int height, CP_FORMAT format>
Sprite<width, height, format>::Sprite (uint8_t* data) :
	FrameBuffer<width, height, format>( &data[9] ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0 ),
	m_RotPointX( width / 2 ),
	m_RotPointY( height / 2 )
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
unsigned int Sprite<width, height, format>::getScaledWidth() const
{
	return std::floor( width * m_ScaleFactor );
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
unsigned int Sprite<width, height, format>::getScaledHeight() const
{
	return std::floor( height * m_ScaleFactor );
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
void Sprite<width, height, format>::setScaleFactor (float scaleFactor)
{
	m_ScaleFactor = scaleFactor;
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
float Sprite<width, height, format>::getScaleFactor() const
{
	return m_ScaleFactor;
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
void Sprite<width, height, format>::setRotationAngle (int degrees)
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

template <unsigned int width, unsigned int height, CP_FORMAT format>
int Sprite<width, height, format>::getRotationAngle() const
{
	return m_RotationDegrees;
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
void Sprite<width, height, format>::setRotationPointX (float x)
{
	m_RotPointX = ( width - 1 ) * x;
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
int Sprite<width, height, format>::getRotationPointX() const
{
	return m_RotPointX;
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
void Sprite<width, height, format>::setRotationPointY (float y)
{
	m_RotPointY = ( height - 1 ) * y;
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
int Sprite<width, height, format>::getRotationPointY() const
{
	return m_RotPointY;
}

#endif // SPRITE_HPP
