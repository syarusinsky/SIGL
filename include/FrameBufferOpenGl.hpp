#ifndef FRAMEBUFFEROPENGL_HPP
#define FRAMEBUFFEROPENGL_HPP

/**************************************************************************
 * The OpenGL implementation of the FrameBuffer class.
**************************************************************************/

#include <GL/glew.h>

#include "SLOGE.hpp"
#include "ColorProfile.hpp"


template <unsigned int width, unsigned int height, CP_FORMAT format>
class FrameBufferOpenGlFixed
{
	public:
		FrameBufferOpenGlFixed();
		virtual ~FrameBufferOpenGlFixed();

		GLuint getFrameBufferObject() { return m_FBO; }
		GLuint getRenderBufferObject() { return m_RBO; }
		GLuint getTexture() { return m_Tex; }

		constexpr unsigned int getWidth() const { return width; }
		constexpr unsigned int getHeight() const { return height; }

	private:
		GLuint 	m_FBO;
		GLuint 	m_RBO;
		GLuint 	m_Tex;
};

template <CP_FORMAT format>
class FrameBufferOpenGlDynamic
{
	public:
		FrameBufferOpenGlDynamic (unsigned int width, unsigned int height);
		FrameBufferOpenGlDynamic (unsigned int width, unsigned int height, uint8_t* pixels);
		virtual ~FrameBufferOpenGlDynamic();

		GLuint getFrameBufferObject() { return m_FBO; }
		GLuint getRenderBufferObject() { return m_RBO; }
		GLuint getTexture() { return m_Tex; }

		unsigned int getWidth() const { return m_Width; }
		unsigned int getHeight() const { return m_Height; }

	private:
		const unsigned int 	m_Width;
		const unsigned int 	m_Height;
		GLuint 			m_FBO;
		GLuint 			m_RBO;
		GLuint 			m_Tex;
};

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBufferOpenGlFixed<width, height, format>::FrameBufferOpenGlFixed() :
	m_FBO( 0 ),
	m_RBO( 0 ),
	m_Tex( 0 )
{
	glGenFramebuffers( 1, &m_FBO );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FBO );

	glGenTextures( 1, &m_Tex );
	glBindTexture( GL_TEXTURE_2D, m_Tex );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Tex, 0 );

	glGenRenderbuffers( 1, &m_RBO );
	glBindRenderbuffer( GL_RENDERBUFFER, m_RBO );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO );

	if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
	{
		SLOG::log( LogLevels::ERROR, LogMethodsE::ERROR, "failed checking framebuffer status", __LINE__, __FILE__ );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
	else
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

template <unsigned int width, unsigned int height, CP_FORMAT format>
FrameBufferOpenGlFixed<width, height, format>::~FrameBufferOpenGlFixed()
{
	glDeleteFramebuffers( 1, &m_FBO );
	glDeleteTextures( 1, &m_Tex );
	glDeleteRenderbuffers( 1, &m_RBO );
}

template <CP_FORMAT format>
FrameBufferOpenGlDynamic<format>::FrameBufferOpenGlDynamic (unsigned int width, unsigned int height) :
	m_Width( width ),
	m_Height( height ),
	m_FBO( 0 ),
	m_RBO( 0 ),
	m_Tex( 0 )
{
	glGenFramebuffers( 1, &m_FBO );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FBO );

	glGenTextures( 1, &m_Tex );
	glBindTexture( GL_TEXTURE_2D, m_Tex );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Tex, 0 );

	glGenRenderbuffers( 1, &m_RBO );
	glBindRenderbuffer( GL_RENDERBUFFER, m_RBO );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO );

	if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
	{
		SLOG::log( LogLevels::ERROR, LogMethodsE::ERROR, "failed checking framebuffer status", __LINE__, __FILE__ );
	}
	else
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

template <CP_FORMAT format>
FrameBufferOpenGlDynamic<format>::FrameBufferOpenGlDynamic (unsigned int width, unsigned int height, uint8_t* pixels) :
	m_Width( width ),
	m_Height( height ),
	m_FBO( 0 ),
	m_RBO( 0 ),
	m_Tex( 0 )
{
	glGenFramebuffers( 1, &m_FBO );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FBO );

	glGenTextures( 1, &m_Tex );
	glBindTexture( GL_TEXTURE_2D, m_Tex );

	if constexpr ( format == CP_FORMAT::RGBA_32BIT )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
	} else if constexpr ( format == CP_FORMAT::RGB_24BIT )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );
	} else if constexpr ( format == CP_FORMAT::BGR_24BIT )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_BGR, m_Width, m_Height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels );
	}else if constexpr ( format == CP_FORMAT::MONOCHROME_1BIT )
	{
		ColorProfile<CP_FORMAT::MONOCHROME_1BIT> colorProfile;
		uint8_t newPixels[ width * height * 3 ];
		for ( unsigned int pixelNum = 0; pixelNum < width * height; pixelNum++ )
		{
			newPixels[(pixelNum * 3) + 0] = colorProfile.getPixel( pixels, pixelNum );
			newPixels[(pixelNum * 3) + 1] = colorProfile.getPixel( pixels, pixelNum );
			newPixels[(pixelNum * 3) + 2] = colorProfile.getPixel( pixels, pixelNum );
		}

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, newPixels );
	} else
	{
		SLOG::log( LogLevels::ERROR, LogMethodsE::ERROR, "Somehow provided an undefined color format", __LINE__, __FILE__ );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	}

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Tex, 0 );

	glGenRenderbuffers( 1, &m_RBO );
	glBindRenderbuffer( GL_RENDERBUFFER, m_RBO );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO );

	if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
	{
		SLOG::log( LogLevels::ERROR, LogMethodsE::ERROR, "failed checking framebuffer status", __LINE__, __FILE__ );
	}
	else
	{
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}

template <CP_FORMAT format>
FrameBufferOpenGlDynamic<format>::~FrameBufferOpenGlDynamic()
{
	glDeleteFramebuffers( 1, &m_FBO );
	glDeleteTextures( 1, &m_Tex );
	glDeleteRenderbuffers( 1, &m_RBO );
}

#endif // FRAMEBUFFEROPENGL_HPP
