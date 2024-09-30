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

template <CP_FORMAT format, RENDER_API api>
class Sprite
{
	public:
		Sprite (const unsigned int width, const unsigned int height);
		Sprite (uint8_t* data);

		unsigned int getScaledWidth() const;
		unsigned int getScaledHeight() const;

		unsigned int getWidth() const;
		unsigned int getHeight() const;

		void setScaleFactor (float scaleFactor);
		float getScaleFactor() const;

		void setRotationAngle (int degrees);
		int getRotationAngle() const;
		void setRotationPointX (float x); // should be between 0.0f and 1.0f
		float getRotationPointX() const;
		void setRotationPointY (float y); // should be between 0.0f and 1.0f
		float getRotationPointY() const;

		Texture<format, api>& getTexture() { return m_Texture; }

	protected:
		Texture<format, api> 	m_Texture;
		float 			m_ScaleFactor;
		int   			m_RotationDegrees;
		float   		m_RotPointX;
		float   		m_RotPointY;
};

template <CP_FORMAT format, RENDER_API api>
Sprite<format, api>::Sprite (const unsigned int width, const unsigned int height) :
	m_Texture( width, height ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0 ),
	m_RotPointX( static_cast<float>(width ) / 2.0f ),
	m_RotPointY( static_cast<float>(height) / 2.0f )
{
}

template <CP_FORMAT format, RENDER_API api>
Sprite<format, api>::Sprite (uint8_t* data) :
	m_Texture( data ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0 ),
	m_RotPointX( static_cast<float>(m_Texture.getWidth() ) / 2.0f ),
	m_RotPointY( static_cast<float>(m_Texture.getHeight()) / 2.0f )
{
}

template <CP_FORMAT format, RENDER_API api>
unsigned int Sprite<format, api>::getWidth() const
{
	return m_Texture.getWidth();
}

template <CP_FORMAT format, RENDER_API api>
unsigned int Sprite<format, api>::getHeight() const
{
	return m_Texture.getHeight();
}

template <CP_FORMAT format, RENDER_API api>
unsigned int Sprite<format, api>::getScaledWidth() const
{
	return std::floor( m_Texture.getWidth() * m_ScaleFactor );
}

template <CP_FORMAT format, RENDER_API api>
unsigned int Sprite<format, api>::getScaledHeight() const
{
	return std::floor( m_Texture.getHeight() * m_ScaleFactor );
}

template <CP_FORMAT format, RENDER_API api>
void Sprite<format, api>::setScaleFactor (float scaleFactor)
{
	m_ScaleFactor = scaleFactor;
}

template <CP_FORMAT format, RENDER_API api>
float Sprite<format, api>::getScaleFactor() const
{
	return m_ScaleFactor;
}

template <CP_FORMAT format, RENDER_API api>
void Sprite<format, api>::setRotationAngle (int degrees)
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

template <CP_FORMAT format, RENDER_API api>
int Sprite<format, api>::getRotationAngle() const
{
	return m_RotationDegrees;
}

template <CP_FORMAT format, RENDER_API api>
void Sprite<format, api>::setRotationPointX (float x)
{
	m_RotPointX = static_cast<float>( m_Texture.getWidth() ) * x;
}

template <CP_FORMAT format, RENDER_API api>
float Sprite<format, api>::getRotationPointX() const
{
	return m_RotPointX;
}

template <CP_FORMAT format, RENDER_API api>
void Sprite<format, api>::setRotationPointY (float y)
{
	m_RotPointY = static_cast<float>( m_Texture.getHeight() ) * y;
}

template <CP_FORMAT format, RENDER_API api>
float Sprite<format, api>::getRotationPointY() const
{
	return m_RotPointY;
}

#endif // SPRITE_HPP
