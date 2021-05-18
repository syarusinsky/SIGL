#ifndef COLORPROFILE_HPP
#define COLORPROFILE_HPP

/**************************************************************************
 * The ColorProfile class defines the color profile of a frame buffer.
 * The CP_FORMAT enum is used to specify the color depth and pixel width
 * of each pixel. The ColorProfile is used by the graphics class to draw
 * pixels in the frame buffer as well as set the color of each pixel.
**************************************************************************/

#include <stdint.h>

struct Color
{
	float m_R = 0.0f;
	float m_G = 0.0f;
	float m_B = 0.0f;
	float m_A = 0.0f;
	bool  m_M = false;

	bool  m_IsMonochrome = false;
	bool  m_HasAlpha = false;
};

enum class CP_FORMAT
{
	MONOCHROME_1BIT,
	RGB_24BIT,
	RGBA_32BIT
};

class SoftwareGraphics;

class ColorProfile
{
	friend SoftwareGraphics;

	public:
		ColorProfile (const CP_FORMAT& format);
		~ColorProfile();

		void putPixel (uint8_t* fbStart, unsigned int fbNumPixels, unsigned int pixelNum);
		Color getPixel (uint8_t* fbStart, unsigned int fbNumPixels, unsigned int pixelNum) const;

		void setColor (float rValue, float gValue, float bValue);
		void setColor (float rValue, float gValue, float bValue, float aValue);
		void setColor (bool mValue, bool useAlpha = false);
		void setColor (const Color& color);
		Color getColor() const;

		const CP_FORMAT getFormat() const;

	private:
		const CP_FORMAT m_Format;

		uint8_t m_RValue;
		uint8_t m_GValue;
		uint8_t m_BValue;
		uint8_t m_AValue;
		bool    m_MValue; // for monochrome
};

#endif // COLOR_PROFILE_HPP
