#ifndef TEXTURE_HPP
#define TEXTURE_HPP

/**************************************************************************
 * The Texture class defines a framebuffer that may act as a texture for
 * 3D texture mapped faces.
**************************************************************************/

#include "FrameBuffer.hpp"
#include <cmath>

template <unsigned int width, unsigned int height, CP_FORMAT format>
class Texture : public FrameBuffer<width, height, format>
{
	public:
		// Texture (unsigned int width, unsigned int height, const CP_FORMAT& format);
		Texture();
		Texture (uint8_t* data);

		const Color getColor (float texCoordX, float texCoordY) const;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
Texture<width, height, format>::Texture() :
	FrameBuffer<width, height, format>()
{
}

// for sif files,...
// width: (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4]
// height: (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]
// format: FormatInitializer( data[0] ).getFormat()
// pixels: &data[9]
template <unsigned int width, unsigned int height, CP_FORMAT format>
Texture<width, height, format>::Texture (uint8_t* data) :
	FrameBuffer<width, height, format>( data )
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
const Color Texture<width, height, format>::getColor (float texCoordX, float texCoordY) const
{
	// wrap texture coordinates from 0 to 1
	texCoordX = std::remainder( texCoordX, 1.0f );
	texCoordY = std::remainder( texCoordY, 1.0f );
	texCoordX = ( texCoordX < 0.0f ) ? 1.0f + texCoordX : texCoordX;
	texCoordY = ( texCoordY < 0.0f ) ? 1.0f + texCoordY : texCoordY;

	const unsigned int xUInt = texCoordX * ( width - 1 );
	const unsigned int yUInt = texCoordY * ( height - 1 );
	const unsigned int pixelNum = (yUInt * width) + xUInt;

	return FrameBuffer<width, height, format>::m_ColorProfile.getPixel<width, height>( FrameBuffer<width, height, format>::m_Pixels, pixelNum );
}

#endif // TEXTURE_HPP
