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

	protected:
		float m_ScaleFactor;
		int   m_RotationDegrees;
		int   m_RotPointX;
		int   m_RotPointY;
};

#endif // SPRITE_HPP
