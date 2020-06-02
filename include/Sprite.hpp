#ifndef SPRITE_HPP
#define SPRITE_HPP

/**************************************************************************
 * The Sprite class defines a framebuffer that also contains scaling
 * and rotation information. A Sprite can be 'blitted' onto another
 * FrameBuffer and will also be scaled/rotated according to the sprites
 * current scaling and rotation values.
**************************************************************************/

#include "FrameBuffer.hpp"

class Sprite : public FrameBuffer
{
	public:
		Sprite (unsigned int width, unsigned int height, const CP_FORMAT& format);
		Sprite (unsigned int width, unsigned int height, const CP_FORMAT& format, uint8_t* pixels);

		unsigned int getScaledWidth() const;
		unsigned int getScaledHeight() const;

		void setScaleFactor (float scaleFactor);
		float getScaleFactor() const;

		void setRotationDegrees (float rotDegrees);
		float getRotationDegrees() const;

	protected:
		float m_ScaleFactor;
		float m_RotationDegrees;
};

#endif // SPRITE_HPP
