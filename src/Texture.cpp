#include "Texture.hpp"

#include <cmath>

Texture::Texture (unsigned int width, unsigned int height, const CP_FORMAT& format) :
	FrameBuffer( width, height, format )
{
}

Texture::Texture (uint8_t* data) :
	FrameBuffer( (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4],
			(data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8],
			FormatInitializer( data[0] ).getFormat(),
			&data[9] )
{
}

Color Texture::getColor (float texCoordX, float texCoordY)
{
	// wrap texture coordinates from 0 to 1
	texCoordX = std::remainder( texCoordX, 1.0f );
	texCoordY = std::remainder( texCoordY, 1.0f );
	texCoordX = ( texCoordX < 0.0f ) ? 1.0f + texCoordX : texCoordX;
	texCoordY = ( texCoordY < 0.0f ) ? 1.0f + texCoordY : texCoordY;

	unsigned int xUInt = texCoordX * ( m_Width - 1 );
	unsigned int yUInt = texCoordY * ( m_Height - 1 );

	return m_ColorProfile.getPixel( m_Pixels, m_Width * m_Height, (yUInt * m_Width) + xUInt );
}
