#ifndef COLORPROFILE_HPP
#define COLORPROFILE_HPP

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
