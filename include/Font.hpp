#ifndef FONT_HPP
#define FONT_HPP

/**************************************************************************
 * The Font class defines a bitmap font that holds a bitmap of all
 * characters. The graphics class can use a Font to blit characters to
 * a frame buffer.
**************************************************************************/

#include <stdint.h>
#include <map>

class Font
{
	public:
		Font (uint8_t* data);

		unsigned int getCharacterWidth() const;
		unsigned int getBitmapWidth() const;
		unsigned int getBitmapHeight() const;

		unsigned int getCharacterIndex (const char character) const; // returns the offset to begin rendering at

		const uint8_t* getBitmapStart() const;

	private:
		const uint8_t*          m_Data;
		const unsigned int      m_CharacterWidth;
		const unsigned int      m_Width;
		const unsigned int      m_Height;
		const unsigned int      m_BitmapStartIndex;
		const uint8_t*          m_BitmapStart;
		std::map<char, uint8_t> m_CharMap;

		void createCharacterMap();
};

#endif // FONT_HPP
