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

inline void openGLOffsetVerts (float& x1, float& y1, float& x2, float& y2, float& x3, float& y3, float& x4, float& y4);
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
		GLuint 	m_BasicSpriteProgram;

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
	m_BasicColorProgram( 0 ),
	m_BasicSpriteProgram( 0 )
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

	const char *basicSpriteVertexShaderSource = 	"#version 330 core\n"
							"\n"
							"layout (location = 0) in vec3 aPos;\n"
							"layout (location = 1) in vec2 aTexCoord;\n"
							"out vec2 TexCoord; \n"
							"void main()\n"
							"{\n"
							"   gl_Position = vec4( aPos.x, aPos.y, aPos.z, 1.0 );\n"
							"   TexCoord = aTexCoord; \n"
							"}";
	const char *basicSpriteFragmentShaderSource = 	"#version 330 core\n"
							"out vec4 FragColor;\n"
							"in vec2 TexCoord;\n"
							"uniform sampler2D SpriteTex; \n"
							"void main()\n"
							"{\n"
							"   FragColor = texture( SpriteTex, TexCoord );\n"
							"}\n\0";

	GLuint basicSpriteVertexShader = glCreateShader( GL_VERTEX_SHADER );
	GLuint basicSpriteFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( basicSpriteVertexShader, 1, &basicSpriteVertexShaderSource, NULL );
	glShaderSource( basicSpriteFragmentShader, 1, &basicSpriteFragmentShaderSource, NULL );
	glCompileShader( basicSpriteVertexShader );
	glGetShaderiv( basicSpriteVertexShader, GL_COMPILE_STATUS, &success );
	if ( ! success )
	{
		char infoLog[512];
		glGetShaderInfoLog( basicSpriteVertexShader, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glCompileShader( basicSpriteFragmentShader );
	glGetShaderiv( basicSpriteFragmentShader, GL_COMPILE_STATUS, &success );
	if ( ! success )
	{
		char infoLog[512];
		glGetShaderInfoLog( basicSpriteFragmentShader, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	m_BasicSpriteProgram = glCreateProgram();
	glAttachShader( m_BasicSpriteProgram, basicSpriteVertexShader );
	glAttachShader( m_BasicSpriteProgram, basicSpriteFragmentShader );
	glLinkProgram( m_BasicSpriteProgram );
	glGetProgramiv( m_BasicSpriteProgram, GL_LINK_STATUS, &success );
	if( ! success )
	{
		char infoLog[512];
		glGetProgramInfoLog( m_BasicSpriteProgram, 512, NULL, infoLog );
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader( basicSpriteVertexShader );
	glDeleteShader( basicSpriteFragmentShader );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::~OpenGlGraphics()
{
	glDeleteProgram( m_BasicColorProgram );
	glDeleteProgram( m_BasicSpriteProgram );
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
	const Color color = m_ColorProfile.template getColor<format>();

	glClearColor( color.m_R, color.m_G, color.m_B, color.m_A );
	glClear( GL_COLOR_BUFFER_BIT );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawLine (float xStart, float yStart, float xEnd, float yEnd)
{
	const Color color = m_ColorProfile.template getColor<format>();

	openGLOffsetVerts( xStart, yStart, xEnd, yEnd );

	// offset so that 0.0f on the x axis and 1.0f on the y axis are still drawn
	if ( xStart == -1.0f )
	{
		xStart += 0.00001f;
	}
	if ( xEnd == -1.0f )
	{
		xEnd += 0.00001f;
	}
	if ( yStart == -1.0f )
	{
		yStart += 0.00001f;
	}
	if ( yEnd == -1.0f )
	{
		yEnd += 0.00001f;
	}

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
	const Color color = m_ColorProfile.template getColor<format>();

	openGLOffsetVerts( xStart, yStart, xEnd, yEnd );

	const float xDist = xEnd - xStart;
	const float yDist = yEnd - yStart;

	const float vertices[] = {
		xStart, yStart, 0.0f,
		xStart + xDist, yStart, 0.0f,
		xStart, yStart + yDist, 0.0f,
		xStart + xDist, yStart, 0.0f,
		xStart, yStart + yDist, 0.0f,
		xEnd, yEnd, 0.0f
	};

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

	const float boxColor[] = { color.m_R, color.m_G, color.m_B, color.m_A };
	glUniform4fv( glGetUniformLocation(m_BasicColorProgram, "color"), 1, &boxColor[0] );

	glDrawArrays( GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float) / 3 );

	glDeleteVertexArrays( 1, &VAO );
	glDeleteBuffers( 1, &VBO );
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
	const Color color = m_ColorProfile.template getColor<format>();

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

	glDrawArrays( GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float) / 3 );

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

template <unsigned int width, unsigned int height>
inline void drawCircleHelper (const GLuint program, const Color& color, float originX, float originY, float radius, bool filled)
{
	openGLOffsetVerts( originX, originY );

	// TODO this should probably be set dynamically
	constexpr unsigned int numSegments = 100;

	// need to squish or expand vertical radius to account for aspect ratio
	radius *= 2.0f;
	const float rW = radius;
	const float rH = radius * static_cast<float>( width ) / static_cast<float>( height );

	float vertices[numSegments * 3];

	for ( int segment = 0; segment < numSegments; segment++ )
	{
		float theta = 2.0f * M_PI * static_cast<float>( segment ) / static_cast<float>( numSegments );
		float x = rW * cosf( theta );
		float y = rH * sinf( theta );
		vertices[(segment * 3) + 0] = x + originX;
		vertices[(segment * 3) + 1] = y + originY;
		vertices[(segment * 3) + 2] = 0.0f;
	}

	unsigned int VAO;
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	unsigned int VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 );
	glEnableVertexAttribArray(0);

	glUseProgram( program );

	const float circleColor[] = { color.m_R, color.m_G, color.m_B, color.m_A };
	glUniform4fv( glGetUniformLocation(program, "color"), 1, &circleColor[0] );

	glDrawArrays( (filled) ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, sizeof(vertices) / sizeof(float) / 3 );

	glDeleteVertexArrays( 1, &VAO );
	glDeleteBuffers( 1, &VBO );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircle (float originX, float originY, float radius)
{
	const Color color = m_ColorProfile.template getColor<format>();

	drawCircleHelper<width, height>( m_BasicColorProgram, color, originX, originY, radius, false );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>::drawCircleFilled (float originX, float originY, float radius)
{
	const Color color = m_ColorProfile.template getColor<format>();

	drawCircleHelper<width, height>( m_BasicColorProgram, color, originX, originY, radius, true );
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
	// define vertices
	float xPoint2 = xStart + ( sprite.getScaledWidth() * (1.0f / width) );
	float yPoint2 = yStart;
	float xPoint3 = xStart;
	float yPoint3 = yStart + ( sprite.getScaledHeight() * (1.0f / height) );
	float xEnd = xPoint2;
	float yEnd = yPoint3;

	// convert to pixel vals
	constexpr float widthF = static_cast<float>( width );
	constexpr float heightF = static_cast<float>( height );
	xStart *= widthF;
	xPoint2 *= widthF;
	xPoint3 *= widthF;
	xEnd *= widthF;
	yStart *= heightF;
	yPoint2 *= heightF;
	yPoint3 *= heightF;
	yEnd *= heightF;

	// offset to rotation point
	const float rotX = sprite.getRotationPointX() * sprite.getScaleFactor();
	const float rotY = sprite.getRotationPointY() * sprite.getScaleFactor();
	const float originalXStart = xStart;
	const float originalYStart = yStart;
	const float xOffsetToRotPoint = xStart + rotX;
	const float yOffsetToRotPoint = yStart + rotY;
	xStart -= xOffsetToRotPoint;
	yStart -= yOffsetToRotPoint;
	xPoint2 -= xOffsetToRotPoint;
	yPoint2 -= yOffsetToRotPoint;
	xPoint3 -= xOffsetToRotPoint;
	yPoint3 -= yOffsetToRotPoint;
	xEnd -= xOffsetToRotPoint;
	yEnd -= yOffsetToRotPoint;

	// variables needed for rotation
	const float radians = ( static_cast<float>(sprite.getRotationAngle()) * M_PI ) * ( 1.0f / 180.0f );
	const float sinVal = sin( radians );
	const float cosVal = cos( radians );

	// rotate vertices
	float xStartRot = ( cosVal * xStart ) - ( sinVal * yStart );
	float yStartRot = ( sinVal * xStart ) + ( cosVal * yStart );
	float xPoint2Rot = ( cosVal * xPoint2 ) - ( sinVal * yPoint2 );
	float yPoint2Rot = ( sinVal * xPoint2 ) + ( cosVal * yPoint2 );
	float xPoint3Rot = ( cosVal * xPoint3 ) - ( sinVal * yPoint3 );
	float yPoint3Rot = ( sinVal * xPoint3 ) + ( cosVal * yPoint3 );
	float xEndRot = ( cosVal * xEnd ) - ( sinVal * yEnd );
	float yEndRot = ( sinVal * xEnd ) + ( cosVal * yEnd );

	// offset back from rotation point
	xStart = xStartRot + originalXStart + sprite.getRotationPointX();
	yStart = yStartRot + originalYStart + sprite.getRotationPointY();
	xPoint2 = xPoint2Rot + originalXStart + sprite.getRotationPointX();
	yPoint2 = yPoint2Rot + originalYStart + sprite.getRotationPointY();
	xPoint3 = xPoint3Rot + originalXStart + sprite.getRotationPointX();
	yPoint3 = yPoint3Rot + originalYStart + sprite.getRotationPointY();
	xEnd = xEndRot + originalXStart + sprite.getRotationPointX();
	yEnd = yEndRot + originalYStart + sprite.getRotationPointY();

	// convert back to relative
	constexpr float oneOverWidthF = 1.0f / widthF;
	constexpr float oneOverHeightF = 1.0f / heightF;
	xStart *= oneOverWidthF;
	xPoint2 *= oneOverWidthF;
	xPoint3 *= oneOverWidthF;
	xEnd *= oneOverWidthF;
	yStart *= oneOverHeightF;
	yPoint2 *= oneOverHeightF;
	yPoint3 *= oneOverHeightF;
	yEnd *= oneOverHeightF;

	openGLOffsetVerts( xStart, yStart, xPoint2, yPoint2, xPoint3, yPoint3, xEnd, yEnd );

	const float vertices[] = {
		xPoint2, yPoint2, 0.0f, 1.0f, 1.0f, 	// top right
		xEnd, yEnd, 0.0f, 1.0f, 0.0f, 		// bottom right
		xPoint3, yPoint3, 0.0f, 0.0f, 0.0f, 	// bottom left
		xStart, yStart, 0.0f, 0.0f, 1.0f 	// top left
	};

	const unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	// generate texture TODO should really be doing this in the sprite class itself
	GLuint texture;
	glGenTextures( 1, &texture );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	// TODO need to compensate for color format
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, sprite.getWidth(), sprite.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, sprite.getData() );

	// draw the sprite TODO should this also be done in the sprite class?
	GLuint VAO;
	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );

	GLuint VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

	GLuint EBO;
	glGenBuffers( 1, &EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0 );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)) );
	glEnableVertexAttribArray( 1 );

	glUseProgram( m_BasicSpriteProgram );

	GLuint textureLocation = glGetUniformLocation( m_BasicSpriteProgram, "SpriteTex" );
	glUniform1i( textureLocation, 0 );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

	glDeleteVertexArrays( 1, &VAO );
	glDeleteBuffers( 1, &VBO );
	glDeleteBuffers( 1, &EBO );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
void OpenGlGraphics3D<width, height, format, api, include3D, shaderPassDataSize>::drawDepthBuffer (Camera3D& camera)
{
	// TODO render depth buffer
}

inline void openGLOffsetVerts (float& x1, float& y1, float& x2, float& y2, float& x3, float& y3, float& x4, float& y4)
{
	x1 *= 2.0f;
	x2 *= 2.0f;
	x3 *= 2.0f;
	x4 *= 2.0f;
	y1 *= -2.0f;
	y2 *= -2.0f;
	y3 *= -2.0f;
	y4 *= -2.0f;
	x1 -= 1.0f;
	x2 -= 1.0f;
	x3 -= 1.0f;
	x4 -= 1.0f;
	y1 += 1.0f;
	y2 += 1.0f;
	y3 += 1.0f;
	y4 += 1.0f;
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
