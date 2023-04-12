#ifndef OPENGLGRAPHICS_HPP
#define OPENGLGRAPHICS_HPP

/**************************************************************************
 * The OpenGlGraphics class defines functions used to render graphics
 * with the OpenGL api rendering api.
**************************************************************************/

#include "IGraphics.hpp"

#include "Font.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"
#include "Engine3D.hpp"

#include <algorithm>
#include <limits>

inline void openGLOffsetVerts (float& x1, float& y1, float& x2, float& y2, float& x3, float& y3);
inline void openGLOffsetVerts (float& x1, float& y1, float& x2, float& y2);
inline void openGLOffsetVerts (float& x1, float& y1);

// just to avoid compilation error
template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class OpenGlGraphicsNo3D : public IGraphics<width, height, format, api, include3D, shaderPassDataSize>
{
	public:
		virtual ~OpenGlGraphicsNo3D() {}
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class OpenGlGraphics3D : public IGraphics<width, height, format, api, include3D, shaderPassDataSize>
{
	public:
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData) override;
		void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData) override;
		void drawDepthBuffer (Camera3D& camera) override;

	protected:
		template <CP_FORMAT texFormat> void drawTriangleShadedHelper (Face& face, TriShaderData<texFormat, shaderPassDataSize>& shaderData);

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_ColorProfile;
		using IGraphics3D<width, height, shaderPassDataSize>::m_DepthBuffer;
		using IGraphics3D<width, height, shaderPassDataSize>::m_ShaderPassData;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_FB;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class OpenGlGraphics 	: public std::conditional<include3D, OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>,
								OpenGlGraphicsNo3D<width, height, format, api, include3D, shaderPassDataSize>>::type
{
	// only a surface should be able to construct
	template<RENDER_API rAPI, unsigned int w, unsigned int h, CP_FORMAT f, unsigned int nT, bool i3D, unsigned int sPDS> friend class SurfaceThreaded;
	template<RENDER_API rAPI, unsigned int w, unsigned int h, CP_FORMAT f, bool i3D, unsigned int sPDS> friend class SurfaceSingleCore;

	public:
		void setColor (float r, float g, float b) override;
		void setColor (bool val) override;

		void setFont (Font* font) override;
		Font* getFont() override;

		void fill() override;
		void drawLine (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawBox (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3) override;
		void drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3) override;
		void drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
		void drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
		void drawCircle (float originX, float originY, float radius) override;
		void drawCircleFilled (float originX, float originY, float radius) override;
		void drawText (float xStart, float yStart, const char* text, float scaleFactor) override;

		void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::MONOCHROME_1BIT>& sprite) override;
		void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGBA_32BIT>& sprite) override;
		void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGB_24BIT>& sprite) override;

	protected:
		GLuint 	m_BasicColorProgram;

		template <typename S>
		void drawSpriteHelper (float xStart, float yStart, S& sprite);

		OpenGlGraphics();
		virtual ~OpenGlGraphics() override;

		void startFrame() override;
		void endFrame() override;

		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_ColorProfile;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_CurrentFont;
		using IGraphics<width, height, format, api, include3D, shaderPassDataSize>::m_FB;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::OpenGlGraphics() :
	m_BasicColorProgram( 0 )
{
	const char *basicColorVertexShaderSource = 	"#version 330 core\n"
							"\n"
							"layout (location = 0) in vec3 aPos;\n"
							"void main()\n"
							"{\n"
							"   gl_Position = vec4( aPos.x, aPos.y, aPos.z, 1.0 );\n"
							"}";
	const char *basicColorFragmentShaderSource = 	"#version 330 core\n"
							"layout (location = 0) out vec4 FragColor;\n"
							"uniform vec4 color;\n"
							"void main()\n"
							"{\n"
							"   FragColor = color;\n"
							"}\n\0";

	GLuint basicColorVertexShader = glCreateShader( GL_VERTEX_SHADER );
	GLuint basicColorFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( basicColorVertexShader, 1, &basicColorVertexShaderSource, NULL );
	glShaderSource( basicColorFragmentShader, 1, &basicColorFragmentShaderSource, NULL );
	glCompileShader( basicColorVertexShader );
	int success;
	glGetShaderiv( basicColorVertexShader, GL_COMPILE_STATUS, &success );
	if ( ! success )
	{
		char infoLog[512];
		glGetShaderInfoLog( basicColorVertexShader, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glCompileShader( basicColorFragmentShader );
	glGetShaderiv( basicColorFragmentShader, GL_COMPILE_STATUS, &success );
	if ( ! success )
	{
		char infoLog[512];
		glGetShaderInfoLog( basicColorFragmentShader, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	m_BasicColorProgram = glCreateProgram();
	glAttachShader( m_BasicColorProgram, basicColorVertexShader );
	glAttachShader( m_BasicColorProgram, basicColorFragmentShader );
	glLinkProgram( m_BasicColorProgram );
	glGetProgramiv( m_BasicColorProgram, GL_LINK_STATUS, &success );
	if( ! success )
	{
		char infoLog[512];
		glGetProgramInfoLog( m_BasicColorProgram, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader( basicColorVertexShader );
	glDeleteShader( basicColorFragmentShader );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::~OpenGlGraphics()
{
	glDeleteProgram( m_BasicColorProgram );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::setColor (float r, float g, float b)
{
	m_ColorProfile.setColor( r, g, b );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::setColor (bool val)
{
	m_ColorProfile.setColor( val );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::setFont (Font* font)
{
	m_CurrentFont = font;
}


template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
Font* OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::getFont()
{
	return m_CurrentFont;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::startFrame()
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_FB.getFrameBufferObject() );

	glViewport( 0, 0, width, height );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::endFrame()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::fill()
{
	const Color color = m_ColorProfile.getColor();

	glClearColor( color.m_R, color.m_G, color.m_B, color.m_A );
	glClear( GL_COLOR_BUFFER_BIT );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawLine (float xStart, float yStart, float xEnd, float yEnd)
{
	const Color color = m_ColorProfile.getColor();

	openGLOffsetVerts( xStart, yStart, xEnd, yEnd );
	const float vertices[] = { xStart, yStart, 0.0f, xEnd, yEnd, 0.0f };

	GLuint VAO;
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	GLuint VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 );
	glEnableVertexAttribArray( 0 );

	glUseProgram( m_BasicColorProgram );

	const float lineColor[] = { color.m_R, color.m_G, color.m_B, color.m_A };
	glUniform4fv( glGetUniformLocation(m_BasicColorProgram, "color"), 1, &lineColor[0] );

	glDrawArrays( GL_LINES, 0, 2 );

	glDeleteVertexArrays( 1, &VAO );
	glDeleteBuffers( 1, &VBO );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawBox (float xStart, float yStart, float xEnd, float yEnd)
{
	drawLine( xStart, yStart, xEnd,   yStart );
	drawLine( xEnd,   yStart, xEnd,   yEnd   );
	drawLine( xEnd,   yEnd,   xStart, yEnd   );
	drawLine( xStart, yEnd,   xStart, yStart );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd)
{
	// TODO clip the box from 0.0 to 1.0, get color from color profile, draw filled box
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x1, y1 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleFilled (float x1, float y1, float x2, float y2, float x3,
		float y3)
{
	const Color color = m_ColorProfile.getColor();

	openGLOffsetVerts( x1, y1, x2, y2, x3, y3 );

	const float vertices[] = {
		x1, y1, 0.0f,
		x2, y2, 0.0f,
		x3,  y3, 0.0f
	};

	unsigned int VAO;
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	unsigned int VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 );
	glEnableVertexAttribArray(0);

	glUseProgram( m_BasicColorProgram );

	const float triColor[] = { color.m_R, color.m_G, color.m_B, color.m_A };
	glUniform4fv( glGetUniformLocation(m_BasicColorProgram, "color"), 1, &triColor[0] );

	glDrawArrays( GL_TRIANGLES, 0, 3 );

	glDeleteVertexArrays( 1, &VAO );
	glDeleteBuffers( 1, &VBO );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::MONOCHROME_1BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face,
		TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGBA_32BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT,
		shaderPassDataSize>& shaderData)
{
	this->drawTriangleShadedHelper<CP_FORMAT::RGB_24BIT>( face, shaderData );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <CP_FORMAT texFormat>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawTriangleShadedHelper (Face& face,
			TriShaderData<texFormat, shaderPassDataSize>& shaderData)
{
	// TODO draw shaded triangle using shaderData
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawQuad (float x1, float y1, float x2, float y2, float x3, float y3,
		float x4, float y4)
{
	drawLine( x1, y1, x2, y2 );
	drawLine( x2, y2, x3, y3 );
	drawLine( x3, y3, x4, y4 );
	drawLine( x4, y4, x1, y1 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3,
		float x4, float y4)
{
	drawTriangleFilled( x1, y1, x2, y2, x3, y3 );
	drawTriangleFilled( x1, y1, x4, y4, x3, y3 );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircle (float originX, float originY, float radius)
{
	// TODO get color from color profile, draw circle
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircleFilled (float originX, float originY, float radius)
{
	// TODO get color from color profile, draw filled circle
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawText (float xStart, float yStart, const char* text, float scaleFactor)
{
	// TODO get color from color profile, draw text
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::MONOCHROME_1BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::MONOCHROME_1BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::RGBA_32BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGBA_32BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSprite (float xStart, float yStart,
		Sprite<CP_FORMAT::RGB_24BIT>& sprite)
{
	this->drawSpriteHelper<Sprite<CP_FORMAT::RGB_24BIT>>( xStart, yStart, sprite );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
template <typename S>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawSpriteHelper (float xStart, float yStart, S& sprite)
{
	// TODO draw sprite
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawDepthBuffer (Camera3D& camera)
{
	// TODO render depth buffer
}

inline void openGLOffsetVerts (float& x1, float& y1, float& x2, float& y2, float& x3, float& y3)
{
	x1 *= 2.0f;
	x2 *= 2.0f;
	x3 *= 2.0f;
	y1 *= -2.0f;
	y2 *= -2.0f;
	y3 *= -2.0f;
	x1 -= 1.0f;
	x2 -= 1.0f;
	x3 -= 1.0f;
	y1 += 1.0f;
	y2 += 1.0f;
	y3 += 1.0f;
}

inline void openGLOffsetVerts (float& x1, float& y1, float& x2, float& y2)
{
	x1 *= 2.0f;
	x2 *= 2.0f;
	y1 *= -2.0f;
	y2 *= -2.0f;
	x1 -= 1.0f;
	x2 -= 1.0f;
	y1 += 1.0f;
	y2 += 1.0f;
}

inline void openGLOffsetVerts (float& x1, float& y1)
{
	x1 *= 2.0f;
	y1 *= -2.0f;
	x1 -= 1.0f;
	y1 += 1.0f;
}

#endif // OPENGLGRAPHICS_HPP
