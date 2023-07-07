#ifndef COLORPROFILE_HPP
#define COLORPROFILE_HPP

/**************************************************************************
 * The ColorProfile class defines the color profile of a frame buffer.
 * The CP_FORMAT enum is used to specify the color depth and pixel width
 * of each pixel. The ColorProfile is used by the graphics class to draw
 * pixels in the frame buffer as well as set the color of each pixel.
**************************************************************************/

#include <stdint.h>
#include <array>
#include <math.h>

enum class RENDER_API
{
	SOFTWARE,
	OPENGL
};

struct Color
{
	float m_R = 0.0f;
	float m_G = 0.0f;
	float m_B = 0.0f;
	float m_A = 0.0f;
	bool  m_M = false;

	bool  m_IsMonochrome = false;
	bool  m_HasAlpha = false;

	Color operator* (float scalar) const { return Color({ m_R * scalar, m_G * scalar, m_B * scalar, m_A * scalar, false, false, true }); }
	Color operator+ (const Color& other) { return Color({ m_R + other.m_R, m_G + other.m_G, m_B + other.m_B, m_A + other.m_A, false, false, true }); }
	Color alphaBlend (const Color& other) { return ( other * (1.0f - m_A) ) + ( *this * m_A ); }
};

enum class CP_FORMAT
{
	MONOCHROME_1BIT,
	RGB_24BIT,
	RGBA_32BIT
};

class ColorProfileCommon
{
	public:
		ColorProfileCommon() :
			m_RValue( 0 ),
			m_GValue( 0 ),
			m_BValue( 0 ),
			m_AValue( 255 ),
			m_MValue( false ) {}

		template <CP_FORMAT format>
		Color getColor() const;

	protected:
		uint8_t m_RValue;
		uint8_t m_GValue;
		uint8_t m_BValue;
		uint8_t m_AValue;
		bool    m_MValue; // for monochrome
};

template <CP_FORMAT format>
class ColorProfileMonochrome : public ColorProfileCommon
{
	public:
		template <unsigned int width, unsigned int height>
		void putPixel (std::array<uint8_t, (width * height) / 8>& pixelArray, unsigned int pixelNum)
		{
#ifdef ROTATE_DISPLAY_180_DEGREES
			pixelNum = fbNumPixels - 1 - pixelNum;
#endif
			unsigned int byteNum = std::floor( pixelNum / 8 );
			unsigned int pixelIndex = 7 - (pixelNum % 8);
			uint8_t bitmask = ( 1 << pixelIndex );
			if ( m_MValue == true && m_AValue > 0 )
			{
				pixelArray[byteNum] = pixelArray[byteNum] | bitmask;
			}
			else if ( m_AValue > 0 )
			{
				pixelArray[byteNum] = pixelArray[byteNum] & ~(bitmask);
			}
		}

		template <unsigned int width, unsigned int height>
		void putPixelWithAlphaBlending (std::array<uint8_t, (width * height) / 8>& pixelArray, unsigned int pixelNum)
		{
#ifdef ROTATE_DISPLAY_180_DEGREES
			pixelNum = fbNumPixels - 1 - pixelNum;
#endif
			unsigned int byteNum = std::floor( pixelNum / 8 );
			unsigned int pixelIndex = 7 - (pixelNum % 8);
			uint8_t bitmask = ( 1 << pixelIndex );
			if ( m_MValue == true && m_AValue > 0 )
			{
				pixelArray[byteNum] = pixelArray[byteNum] | bitmask;
			}
			else if ( m_AValue > 0 )
			{
				pixelArray[byteNum] = pixelArray[byteNum] & ~(bitmask);
			}
		}

		template <unsigned int width, unsigned int height>
		Color getPixel (std::array<uint8_t, (width * height) / 8>& pixelArray, unsigned int pixelNum)
		{
			Color color;

			color.m_IsMonochrome = true;

			unsigned int byteNum = std::floor( pixelNum / 8 );
			unsigned int pixelIndex = 7 - (pixelNum % 8);
			uint8_t bitmask = ( 1 << pixelIndex );

			uint8_t byte = pixelArray[byteNum];

			if ( (byte & bitmask) >> pixelIndex )
			{
				color.m_M = true;
				color.m_R = 1.0f;
				color.m_G = 1.0f;
				color.m_B = 1.0f;
				color.m_A = 1.0f;
			}
			else
			{
				color.m_M = false;
				color.m_R = 0.0f;
				color.m_G = 0.0f;
				color.m_B = 0.0f;
				color.m_A = 0.0f;
			}

			color.m_HasAlpha = false;

			return color;
		}

		Color getPixel (uint8_t* pixels, unsigned int pixelNum)
		{
			Color color;

			color.m_IsMonochrome = true;

			unsigned int byteNum = std::floor( pixelNum / 8 );
			unsigned int pixelIndex = 7 - (pixelNum % 8);
			uint8_t bitmask = ( 1 << pixelIndex );

			uint8_t byte = pixels[byteNum];

			if ( (byte & bitmask) >> pixelIndex )
			{
				color.m_M = true;
				color.m_R = 1.0f;
				color.m_G = 1.0f;
				color.m_B = 1.0f;
				color.m_A = 1.0f;
			}
			else
			{
				color.m_M = false;
				color.m_R = 0.0f;
				color.m_G = 0.0f;
				color.m_B = 0.0f;
				color.m_A = 0.0f;
			}

			color.m_HasAlpha = false;

			return color;
		}
};

template <CP_FORMAT format>
class ColorProfileRGB : public ColorProfileCommon
{
	public:

		template <unsigned int width, unsigned int height>
		void putPixel (std::array<uint8_t, width * height * 3>& pixelArray, unsigned int pixelNum)
		{
#ifdef ROTATE_DISPLAY_180_DEGREES
			pixelNum = fbNumPixels - 1 - pixelNum;
#endif

			pixelArray[(pixelNum * 3) + 0] = m_RValue; // Red
			pixelArray[(pixelNum * 3) + 1] = m_GValue; // Green
			pixelArray[(pixelNum * 3) + 2] = m_BValue; // Blue
		}

		template <unsigned int width, unsigned int height>
		void putPixelWithAlphaBlending (std::array<uint8_t, width * height * 3>& pixelArray, unsigned int pixelNum)
		{
#ifdef ROTATE_DISPLAY_180_DEGREES
			pixelNum = fbNumPixels - 1 - pixelNum;
#endif
			Color newColor = getColor<format>().alphaBlend( getPixel<width, height>(pixelArray, pixelNum) );

			pixelArray[(pixelNum * 3) + 0] = 255 * newColor.m_R; // Red
			pixelArray[(pixelNum * 3) + 1] = 255 * newColor.m_G; // Green
			pixelArray[(pixelNum * 3) + 2] = 255 * newColor.m_B; // Blue
		}

		template <unsigned int width, unsigned int height>
		Color getPixel (std::array<uint8_t, width * height * 3>& pixelArray, unsigned int pixelNum)
		{
			Color color;

			color.m_IsMonochrome = false;

			color.m_R = static_cast<float>( pixelArray[(pixelNum * 3 ) + 0]) * (1.0f / 255.0f);
			color.m_G = static_cast<float>( pixelArray[(pixelNum * 3 ) + 1]) * (1.0f / 255.0f);
			color.m_B = static_cast<float>( pixelArray[(pixelNum * 3 ) + 2]) * (1.0f / 255.0f);
			color.m_A = 1.0f;
			color.m_M = true;

			color.m_HasAlpha = false;

			return color;
		}

		Color getPixel (uint8_t* pixels, unsigned int pixelNum)
		{
			Color color;

			color.m_IsMonochrome = false;

			color.m_R = static_cast<float>( pixels[(pixelNum * 3 ) + 0]) * (1.0f / 255.0f);
			color.m_G = static_cast<float>( pixels[(pixelNum * 3 ) + 1]) * (1.0f / 255.0f);
			color.m_B = static_cast<float>( pixels[(pixelNum * 3 ) + 2]) * (1.0f / 255.0f);
			color.m_A = 1.0f;
			color.m_M = true;

			color.m_HasAlpha = false;

			return color;
		}
};

template <CP_FORMAT format>
class ColorProfileRGBA : public ColorProfileCommon
{
	public:
		template <unsigned int width, unsigned int height>
		void putPixel (std::array<uint8_t, width * height * 4>& pixelArray, unsigned int pixelNum)
		{
#ifdef ROTATE_DISPLAY_180_DEGREES
			pixelNum = fbNumPixels - 1 - pixelNum;
#endif

			pixelArray[(pixelNum * 4) + 0] = m_RValue; // Red
			pixelArray[(pixelNum * 4) + 1] = m_GValue; // Green
			pixelArray[(pixelNum * 4) + 2] = m_BValue; // Blue
			pixelArray[(pixelNum * 4) + 3] = m_AValue; // Alpha
		}

		template <unsigned int width, unsigned int height>
		void putPixelWithAlphaBlending (std::array<uint8_t, width * height * 3>& pixelArray, unsigned int pixelNum)
		{
#ifdef ROTATE_DISPLAY_180_DEGREES
			pixelNum = fbNumPixels - 1 - pixelNum;
#endif
			Color newColor = getColor<format>().alphaBlend( getPixel<width, height>(pixelArray, pixelNum) );

			pixelArray[(pixelNum * 4) + 0] = 255 * newColor.m_R; // Red
			pixelArray[(pixelNum * 4) + 1] = 255 * newColor.m_G; // Green
			pixelArray[(pixelNum * 4) + 2] = 255 * newColor.m_B; // Blue
			pixelArray[(pixelNum * 4) + 3] = 255 * newColor.m_B; // Blue
		}

		template <unsigned int width, unsigned int height>
		Color getPixel (std::array<uint8_t, width * height * 4>& pixelArray, unsigned int pixelNum)
		{
			Color color;

			color.m_IsMonochrome = false;

			color.m_R = static_cast<float>( pixelArray[(pixelNum * 4 ) + 0]) * (1.0f / 255.0f);
			color.m_G = static_cast<float>( pixelArray[(pixelNum * 4 ) + 1]) * (1.0f / 255.0f);
			color.m_B = static_cast<float>( pixelArray[(pixelNum * 4 ) + 2]) * (1.0f / 255.0f);
			color.m_A = static_cast<float>( pixelArray[(pixelNum * 4 ) + 3]) * (1.0f / 255.0f);
			color.m_M = true;

			color.m_HasAlpha = true;

			return color;
		}

		Color getPixel (uint8_t* pixels, unsigned int pixelNum)
		{
			Color color;

			color.m_IsMonochrome = false;

			color.m_R = static_cast<float>( pixels[(pixelNum * 4 ) + 0]) * (1.0f / 255.0f);
			color.m_G = static_cast<float>( pixels[(pixelNum * 4 ) + 1]) * (1.0f / 255.0f);
			color.m_B = static_cast<float>( pixels[(pixelNum * 4 ) + 2]) * (1.0f / 255.0f);
			color.m_A = static_cast<float>( pixels[(pixelNum * 4 ) + 3]) * (1.0f / 255.0f);
			color.m_M = true;

			color.m_HasAlpha = true;

			return color;
		}
};

template <CP_FORMAT format>
class ColorProfile : public std::conditional<format == CP_FORMAT::RGB_24BIT, ColorProfileRGB<format>,

				typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, ColorProfileMonochrome<format>,
				ColorProfileRGBA<format>>::type

				>::type
{
	public:
		ColorProfile();
		~ColorProfile();

		void setColor (float rValue, float gValue, float bValue);
		void setColor (float rValue, float gValue, float bValue, float aValue);
		void setColor (bool mValue, bool useAlpha = false);
		void setColor (const Color& color);

		const CP_FORMAT getFormat() const { return format; }
};

template <CP_FORMAT format>
ColorProfile<format>::ColorProfile() :
	std::conditional<format == CP_FORMAT::RGB_24BIT, ColorProfileRGB<format>,

	typename std::conditional<format == CP_FORMAT::MONOCHROME_1BIT, ColorProfileMonochrome<format>,
	ColorProfileRGBA<format>>::type

	>::type()
{
}

template <CP_FORMAT format>
ColorProfile<format>::~ColorProfile()
{
}

template <CP_FORMAT format>
void ColorProfile<format>::setColor (float rValue, float gValue, float bValue)
{
	if ( rValue > 1.0f ) rValue = 1.0f; if ( rValue < 0.0f ) rValue = 0.0f;
	if ( gValue > 1.0f ) gValue = 1.0f; if ( gValue < 0.0f ) gValue = 0.0f;
	if ( bValue > 1.0f ) bValue = 1.0f; if ( bValue < 0.0f ) bValue = 0.0f;

	ColorProfileCommon::m_RValue = std::round( 255 * rValue );
	ColorProfileCommon::m_GValue = std::round( 255 * gValue );
	ColorProfileCommon::m_BValue = std::round( 255 * bValue );
	ColorProfileCommon::m_AValue = 255;

	if ( ColorProfileCommon::m_RValue > 0.0f || ColorProfileCommon::m_GValue > 0.0f || ColorProfileCommon::m_BValue > 0.0f )
	{
		ColorProfileCommon::m_MValue = true;
	}
	else
	{
		ColorProfileCommon::m_MValue = false;
	}
}

template <CP_FORMAT format>
void ColorProfile<format>::setColor (float rValue, float gValue, float bValue, float aValue)
{
	if ( rValue > 1.0f ) rValue = 1.0f; if ( rValue < 0.0f ) rValue = 0.0f;
	if ( gValue > 1.0f ) gValue = 1.0f; if ( gValue < 0.0f ) gValue = 0.0f;
	if ( bValue > 1.0f ) bValue = 1.0f; if ( bValue < 0.0f ) bValue = 0.0f;
	if ( aValue > 1.0f ) aValue = 1.0f; if ( aValue < 0.0f ) aValue = 0.0f;

	ColorProfileCommon::m_RValue = std::round( 255 * rValue );
	ColorProfileCommon::m_GValue = std::round( 255 * gValue );
	ColorProfileCommon::m_BValue = std::round( 255 * bValue );
	ColorProfileCommon::m_AValue = std::round( 255 * aValue );

	if ( ColorProfileCommon::m_RValue > 0.0f || ColorProfileCommon::m_GValue > 0.0f || ColorProfileCommon::m_BValue > 0.0f )
	{
		ColorProfileCommon::m_MValue = true;
	}
	else
	{
		ColorProfileCommon::m_MValue = false;
	}
}

template <CP_FORMAT format>
void ColorProfile<format>::setColor (bool mValue, bool useAlpha)
{
	ColorProfileCommon::m_MValue = mValue;

	if ( mValue )
	{
		ColorProfileCommon::m_RValue = 255;
		ColorProfileCommon::m_GValue = 255;
		ColorProfileCommon::m_BValue = 255;
		ColorProfileCommon::m_AValue = 255;
	}
	else if ( !mValue && useAlpha )
	{
		ColorProfileCommon::m_RValue = 0;
		ColorProfileCommon::m_GValue = 0;
		ColorProfileCommon::m_BValue = 0;
		ColorProfileCommon::m_AValue = 0;
	}
	else
	{
		ColorProfileCommon::m_RValue = 0;
		ColorProfileCommon::m_GValue = 0;
		ColorProfileCommon::m_BValue = 0;
		ColorProfileCommon::m_AValue = 255;
	}
}

template <CP_FORMAT format>
void ColorProfile<format>::setColor (const Color& color)
{
	ColorProfileCommon::m_RValue = 255 * color.m_R;
	ColorProfileCommon::m_GValue = 255 * color.m_G;
	ColorProfileCommon::m_BValue = 255 * color.m_B;
	ColorProfileCommon::m_AValue = 255 * color.m_A;
	ColorProfileCommon::m_MValue = color.m_M;
}

template <CP_FORMAT format>
Color ColorProfileCommon::getColor() const
{
	Color currentColor;
	currentColor.m_R = ((float)ColorProfileCommon::m_RValue * (1.0f / 255.0f));
	currentColor.m_G = ((float)ColorProfileCommon::m_GValue * (1.0f / 255.0f));
	currentColor.m_B = ((float)ColorProfileCommon::m_BValue * (1.0f / 255.0f));
	currentColor.m_A = ((float)ColorProfileCommon::m_AValue * (1.0f / 255.0f));
	currentColor.m_M = ColorProfileCommon::m_MValue;
	currentColor.m_IsMonochrome = ( format == CP_FORMAT::MONOCHROME_1BIT ) ? true : false;
	currentColor.m_HasAlpha = ( format == CP_FORMAT::RGBA_32BIT ) ? true : false;

	return currentColor;
}

#endif // COLOR_PROFILE_HPP
