#include "Font.hpp"

static const unsigned int CHAR_WIDTH_INDEX = 0;
static const unsigned int WIDTH_INDEX_1 = 1;
static const unsigned int WIDTH_INDEX_2 = 2;
static const unsigned int WIDTH_INDEX_3 = 3;
static const unsigned int WIDTH_INDEX_4 = 4;
static const unsigned int HEIGHT_INDEX = 5;
static const unsigned int BITMAP_START_INDEX_INDEX = 6;
static const unsigned int MAPPING_START_INDEX = 7;

Font::Font (uint8_t* data) :
	m_Data( data ),
	m_CharacterWidth( data[CHAR_WIDTH_INDEX] ),
	m_Width( (data[WIDTH_INDEX_1] << 24) | (data[WIDTH_INDEX_2] << 16) | (data[WIDTH_INDEX_3] << 8) | data[WIDTH_INDEX_4] ),
	m_Height( data[HEIGHT_INDEX] ),
	m_MappingStart( &data[MAPPING_START_INDEX] ),
	m_BitmapStartIndex( data[BITMAP_START_INDEX_INDEX] ),
	m_BitmapStart( &data[m_BitmapStartIndex] ),
	m_CharMap()
{
	this->createCharacterMap();
}

unsigned int Font::getCharacterWidth() const
{
	return m_CharacterWidth;
}

unsigned int Font::getBitmapWidth() const
{
	return m_Width;
}

unsigned int Font::getBitmapHeight() const
{
	return m_Height;
}

unsigned int Font::getCharacterIndex (const char character) const
{
	return m_CharMap.at( character );
}

void Font::createCharacterMap()
{
	unsigned int currentMappingIndex;
	for ( currentMappingIndex = MAPPING_START_INDEX; currentMappingIndex < m_BitmapStartIndex; currentMappingIndex += 2 )
	{
		m_CharMap.emplace( m_Data[currentMappingIndex], m_Data[currentMappingIndex + 1] );
	}
}

const uint8_t* Font::getBitmapStart() const
{
	return m_BitmapStart;
}
