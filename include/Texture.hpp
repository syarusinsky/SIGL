#ifndef TEXTURE_HPP
#define TEXTURE_HPP

/**************************************************************************
 * The Texture class defines a framebuffer that may act as a texture for
 * 3D texture mapped faces.
**************************************************************************/

#include "FrameBuffer.hpp"
#include <cmath>

template <CP_FORMAT format, RENDER_API api>
class Texture : public FrameBufferDynamic<format, api>
{
	public:
		Texture (const unsigned int width, const unsigned int height);
		Texture (uint8_t* data);

		Color getColor (float texCoordX, float texCoordY) const;
		Color getColor (unsigned int texCoordX, unsigned int texCoordY) const;
};

template <CP_FORMAT format, RENDER_API api>
Texture<format, api>::Texture (const unsigned int width, const unsigned int height) :
	FrameBufferDynamic<format, api>( width, height )
{
}

// for sif files,...
// width: (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4]
// height: (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]
// format: FormatInitializer( data[0] ).getFormat()
// pixels: &data[9]
template <CP_FORMAT format, RENDER_API api>
Texture<format, api>::Texture (uint8_t* data) :
	FrameBufferDynamic<format, api>(
			(data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4], // width
			(data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8], // height
			&data[9] ) // pixels
{
}

template <CP_FORMAT format, RENDER_API api>
Color Texture<format, api>::getColor (float texCoordX, float texCoordY) const
{
	// ensure that the texure coords start from the bottom
	texCoordY = 1.0f - texCoordY;

	// wrap texture coordinates from 0 to 1
	texCoordX = std::remainder( texCoordX, 1.0f );
	texCoordY = std::remainder( texCoordY, 1.0f );
	texCoordX = ( texCoordX < 0.0f ) ? 1.0f + texCoordX : texCoordX;
	texCoordY = ( texCoordY < 0.0f ) ? 1.0f + texCoordY : texCoordY;

	unsigned int xInt = ( texCoordX * (this->getWidth() ) );
	unsigned int yInt = ( texCoordY * (this->getHeight()) );

	return FrameBufferDynamic<format, api>::getColor( xInt, yInt );
}

template <CP_FORMAT format, RENDER_API api>
Color Texture<format, api>::getColor (unsigned int texCoordX, unsigned int texCoordY) const
{
	return FrameBufferDynamic<format, api>::getColor( texCoordX, texCoordY );
}

#endif // TEXTURE_HPP
