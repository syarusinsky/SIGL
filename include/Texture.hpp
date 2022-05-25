#ifndef TEXTURE_HPP
#define TEXTURE_HPP

/**************************************************************************
 * The Texture class defines a framebuffer that may act as a texture for
 * 3D texture mapped faces.
**************************************************************************/

#include "FrameBuffer.hpp"
#include <cmath>

template <CP_FORMAT format>
class Texture : public FrameBufferDynamic<format>
{
	public:
		Texture (const unsigned int width, const unsigned int height);
		Texture (uint8_t* data);

		const Color getColor (float texCoordX, float texCoordY);
		const Color getColor (unsigned int texCoordX, unsigned int texCoordY);

	private:
		ColorProfile<format> m_ColorProfile;
};

template <CP_FORMAT format>
Texture<format>::Texture (const unsigned int width, const unsigned int height) :
	FrameBufferDynamic<format>( width, height ),
	m_ColorProfile()
{
}

// for sif files,...
// width: (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4]
// height: (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]
// format: FormatInitializer( data[0] ).getFormat()
// pixels: &data[9]
template <CP_FORMAT format>
Texture<format>::Texture (uint8_t* data) :
	FrameBufferDynamic<format>(
			(data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4], // width
			(data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8], // height
			&data[9] ), // pixels
	m_ColorProfile()
{
}

template <CP_FORMAT format>
const Color Texture<format>::getColor (float texCoordX, float texCoordY)
{
	// wrap texture coordinates from 0 to 1
	texCoordX = std::remainder( texCoordX, 1.0f );
	texCoordY = std::remainder( texCoordY, 1.0f );
	texCoordX = ( texCoordX < 0.0f ) ? 1.0f + texCoordX : texCoordX;
	texCoordY = ( texCoordY < 0.0f ) ? 1.0f + texCoordY : texCoordY;

	return FrameBufferDynamic<format>::getColor( texCoordX, texCoordY );
}

template <CP_FORMAT format>
const Color Texture<format>::getColor (unsigned int texCoordX, unsigned int texCoordY)
{
	return FrameBufferDynamic<format>::getColor( texCoordX, texCoordY );
}

#endif // TEXTURE_HPP
