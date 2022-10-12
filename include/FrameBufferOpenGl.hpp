#include <GL/glew.h>

#include "ColorProfile.hpp"

// TODO remove after testing
#include <iostream>

template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferOpenGl
{
	public:
		FrameBufferOpenGl();
		virtual ~FrameBufferOpenGl();

		GLuint getFrameBufferObject() { return m_FBO; }
		GLuint getTexture() { return m_Tex; }

		constexpr unsigned int getWidth() const { return width; }
		constexpr unsigned int getHeight() const { return height; }

	private:
		GLuint 	m_FBO;
		GLuint 	m_Tex;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBufferOpenGl<width, height, format>::FrameBufferOpenGl() :
	m_FBO( 0 )
{
	glGenFramebuffers( 1, &m_FBO );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FBO );

	glGenTextures( 1, &m_Tex );
	glBindTexture( GL_TEXTURE_2D, m_Tex );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Tex, 0 );

	if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
	{
		std::cout << "SOMETHING WEIRD HAPPENED :(" << std::endl;
	}
	else
	{
		// TODO remove after testing, clearing to red
		glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBufferOpenGl<width, height, format>::~FrameBufferOpenGl()
{
	glDeleteFramebuffers( 1, &m_FBO );
	glDeleteTextures( 1, &m_Tex );
}
