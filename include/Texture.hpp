#ifndef TEXTURE_HPP
#define TEXTURE_HPP

/**************************************************************************
 * The Texture class defines a framebuffer that may act as a texture for
 * 3D texture mapped faces.
**************************************************************************/

#include "FrameBuffer.hpp"

class Texture : public FrameBuffer
{
	public:
		Texture (unsigned int width, unsigned int height, const CP_FORMAT& format);
		Texture (uint8_t* data);

		Color getColor (float texCoordX, float texCoordY);
};

#endif // TEXTURE_HPP
