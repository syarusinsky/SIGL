#ifndef SPRITE_HPP
#define SPRITE_HPP

/**************************************************************************
 * The Sprite class defines a framebuffer that also contains scaling
 * and rotation information. A Sprite can be 'blitted' onto another
 * FrameBuffer and will also be scaled/rotated according to the sprites
 * current scaling and rotation values.
**************************************************************************/

#include "Texture.hpp"

#include <math.h>

template <CP_FORMAT format>
class Sprite : public FrameBufferDynamic<format>
{
	public:
		Sprite (const unsigned int width, const unsigned int height);
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

		Texture<format>& getTexture() { return m_Texture; }

	protected:
		Texture<format> 	m_Texture;
		float 			m_ScaleFactor;
		int   			m_RotationDegrees;
		int   			m_RotPointX;
		int   			m_RotPointY;
};

template <CP_FORMAT format>
Sprite<format>::Sprite (const unsigned int width, const unsigned int height) :
	FrameBufferDynamic<format>( width, height ),
	m_Texture( width, height ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0 ),
	m_RotPointX( width / 2 ),
	m_RotPointY( height / 2 )
{
}

template <CP_FORMAT format>
Sprite<format>::Sprite (uint8_t* data) :
	FrameBufferDynamic<format>( (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4],
					(data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8], &data[9] ),
	m_Texture( data ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0 ),
	m_RotPointX( this->getWidth() / 2 ),
	m_RotPointY( this->getHeight() / 2 )
{
}

template <CP_FORMAT format>
unsigned int Sprite<format>::getScaledWidth() const
{
	return std::floor( this->getWidth() * m_ScaleFactor );
}

template <CP_FORMAT format>
unsigned int Sprite<format>::getScaledHeight() const
{
	return std::floor( this->getHeight() * m_ScaleFactor );
}

template <CP_FORMAT format>
void Sprite<format>::setScaleFactor (float scaleFactor)
{
	m_ScaleFactor = scaleFactor;
}

template <CP_FORMAT format>
float Sprite<format>::getScaleFactor() const
{
	return m_ScaleFactor;
}

template <CP_FORMAT format>
void Sprite<format>::setRotationAngle (int degrees)
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

template <CP_FORMAT format>
int Sprite<format>::getRotationAngle() const
{
	return m_RotationDegrees;
}

template <CP_FORMAT format>
void Sprite<format>::setRotationPointX (float x)
{
	m_RotPointX = ( this->getWidth() - 1 ) * x;
}

template <CP_FORMAT format>
int Sprite<format>::getRotationPointX() const
{
	return m_RotPointX;
}

template <CP_FORMAT format>
void Sprite<format>::setRotationPointY (float y)
{
	m_RotPointY = ( this->getHeight() - 1 ) * y;
}

template <CP_FORMAT format>
int Sprite<format>::getRotationPointY() const
{
	return m_RotPointY;
}

#endif // SPRITE_HPP
