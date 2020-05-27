#ifndef COLORPROFILE_HPP
#define COLORPROFILE_HPP

/**************************************************************************
 * The ColorProfile class defines the color profile of a frame buffer.
 * The CP_FORMAT enum is used to specify the color depth and pixel width
 * of each pixel. The ColorProfile is used by the graphics class to draw
 * pixels in the frame buffer as well as set the color of each pixel.
**************************************************************************/

#include <stdint.h>

enum class CP_FORMAT
{
	MONOCHROME_1BIT,
	RGB_24BIT
};

class ColorProfile
{
	public:
		ColorProfile (const CP_FORMAT& format);
		~ColorProfile();

		void putPixel (uint8_t* fbStart, unsigned int pixelNum);

		void setColor (float rValue, float gValue, float bValue);
		void setColor (bool mValue);

		CP_FORMAT getFormat();

	private:
		CP_FORMAT m_Format;

		uint8_t m_RValue;
		uint8_t m_GValue;
		uint8_t m_BValue;
		bool    m_MValue; // for monochrome
};

#endif // COLOR_PROFILE_HPP
