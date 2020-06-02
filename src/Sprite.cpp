#include "Sprite.hpp"

#include <math.h>

Sprite::Sprite (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	FrameBuffer( width, height, format ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0.0f )
{
}

Sprite::Sprite (unsigned int width, unsigned int height, const CP_FORMAT& format, uint8_t* pixels) :
	FrameBuffer( width, height, format, pixels ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0.0f )
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

void Sprite::setRotationDegrees (float rotDegrees)
{
	m_RotationDegrees = rotDegrees;
}

float Sprite::getRotationDegrees() const
{
	return m_RotationDegrees;
}
