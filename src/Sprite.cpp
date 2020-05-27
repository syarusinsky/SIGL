#include "Sprite.hpp"

#include <math.h>

Sprite::Sprite (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	FrameBuffer( width, height, format ),
	m_ScaleFactor( 1.0f ),
	m_RotationDegrees( 0.0f )
{
}

unsigned int Sprite::getScaledWidth()
{
	return std::floor( m_Width * m_ScaleFactor );
}

unsigned int Sprite::getScaledHeight()
{
	return std::floor( m_Height * m_ScaleFactor );
}

void Sprite::setScaleFactor (float scaleFactor)
{
	m_ScaleFactor = scaleFactor;
}

float Sprite::getScaleFactor()
{
	return m_ScaleFactor;
}

void Sprite::setRotationDegrees (float rotDegrees)
{
	m_RotationDegrees = rotDegrees;
}

float Sprite::getRotationDegrees()
{
	return m_RotationDegrees;
}
