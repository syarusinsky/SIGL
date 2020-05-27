#include "Font.hpp"

static const unsigned int CHAR_WIDTH_INDEX = 0;
static const unsigned int WIDTH_INDEX = 1;
static const unsigned int HEIGHT_INDEX = 2;
static const unsigned int BITMAP_START_INDEX_INDEX = 3;
static const unsigned int MAPPING_START_INDEX = 4;

Font::Font (uint8_t* data) :
	m_Data( data ),
	m_CharacterWidth( data[CHAR_WIDTH_INDEX] ),
	m_Width( data[WIDTH_INDEX] ),
	m_Height( data[HEIGHT_INDEX] ),
	m_MappingStart( &data[MAPPING_START_INDEX] ),
	m_BitmapStartIndex( data[BITMAP_START_INDEX_INDEX] ),
	m_BitmapStart( &data[m_BitmapStartIndex] ),
	m_CharMap()
{
	this->createCharacterMap();
}

unsigned int Font::getCharacterWidth()
{
	return m_CharacterWidth;
}

unsigned int Font::getBitmapWidth()
{
	return m_Width;
}

unsigned int Font::getBitmapHeight()
{
	return m_Height;
}

unsigned int Font::getCharacterIndex (const char character)
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

const uint8_t* Font::getBitmapStart()
{
	return m_BitmapStart;
}
