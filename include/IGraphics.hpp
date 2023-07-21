#ifndef IGRAPHICS_HPP
#define IGRAPHICS_HPP

/*************************************************************************
 * The IGraphics interface defines several helper methods to draw graphical
 * elements to a frame buffer. The IGraphics constructor is protected (as
 * should be the subclasses that define the implementation), so that
 * only a Surface class can construct them. This is so that typically all
 * drawing code will be done inside a surface. The class is abstract,
 * so that both a software rendering version and a hardware accelerated
 * version can be implemented.
*************************************************************************/

#define _USE_MATH_DEFINES

#include "GraphicsHelpers.hpp"
#include "FrameBuffer.hpp"
#include "ColorProfile.hpp"
#include "Sprite.hpp"
#include "Texture.hpp"
#include <string>
#include <math.h>
#include <vector>

class Font;
class Camera3D;
struct Face;
struct PointLight;

#ifdef SOFTWARE_RENDERING
#define VSHADER void (*vShader)(TriShaderData<format, shaderPassDataSize>& vShaderData)
#define FSHADER void (*fShader)(Color& colorOut, TriShaderData<format, shaderPassDataSize>& fShaderData, float v1Cur, float v2Cur, float v3Cur, float texCoordX, float texCoordY, float lightAmnt)
#else
#define VSHADER void (*vShader)(TriShaderData<format, shaderPassDataSize>& vShaderData)
#define FSHADER void (*fShader)(Color& colorOut, TriShaderData<format, shaderPassDataSize>& fShaderData, float v1Cur, float v2Cur, float v3Cur, float texCoordX, float texCoordY, float lightAmnt)
// TODO use classes for hardware accelerated shaders (and maybe software rendered shaders too?)
// class VShader;
// class FShader;
// #define VSHADER VShader*
// #define FSHADER FShader*
#endif // SOFTWARE_RENDERING

template <CP_FORMAT format, unsigned int shaderPassDataSize>
struct TriShaderData
{
	std::array<Texture<format>*, 5>& textures;
	Camera3D& camera;
	Color color;
	std::vector<PointLight>* lights;
	VSHADER;
	FSHADER;
};

// just to avoid compilation error
class IGraphicsNo3D
{
	public:
		virtual ~IGraphicsNo3D() {}
};

template <unsigned int width, unsigned int height, unsigned int shaderPassDataSize>
class IGraphics3D
{
	public:
		virtual ~IGraphics3D() {}

		virtual void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::MONOCHROME_1BIT, shaderPassDataSize>& shaderData) = 0;
		virtual void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGBA_32BIT, shaderPassDataSize>& shaderData) = 0;
		virtual void drawTriangleShaded (Face& face, TriShaderData<CP_FORMAT::RGB_24BIT, shaderPassDataSize>& shaderData) = 0;
		virtual void drawDepthBuffer (Camera3D& camera) = 0;
		virtual void clearDepthBuffer() = 0;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class IGraphics : public std::conditional<include3D, IGraphics3D<width, height, shaderPassDataSize>, IGraphicsNo3D>::type
{
	public:
		inline unsigned int convertXPercentageToUInt (float x) { return x * (width  - 1); }
		inline unsigned int convertYPercentageToUInt (float y) { return y * (height - 1); }

		virtual void setColor (float r, float g, float b) = 0;
		virtual void setColor (bool val) = 0;

		virtual void setFont (Font* font) = 0;
		virtual Font* getFont() = 0;

		virtual void fill() = 0;
		virtual void drawLine (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawBox (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawCircle (float originX, float originY, float radius) = 0;
		virtual void drawCircleFilled (float originX, float originY, float radius) = 0;
		virtual void drawText (float xStart, float yStart, const char* text, float scaleFactor) = 0;

		virtual void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::MONOCHROME_1BIT>& sprite) = 0;
		virtual void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGBA_32BIT>& sprite) = 0;
		virtual void drawSprite (float xStart, float yStart, Sprite<CP_FORMAT::RGB_24BIT>& sprite) = 0;

		inline static float distance (float x1, float y1, float x2, float y2) { return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2)); }

		FrameBufferFixed<width, height, format, api>& getFrameBuffer() { return m_FB; }
		const ColorProfile<format>& getColorProfile() const { return m_ColorProfile; }

	protected:
		FrameBufferFixed<width, height, format, api> 	m_FB;
		ColorProfile<format> 				m_ColorProfile;
		Font* 						m_CurrentFont;

		// helpers
		static inline bool approxEqual (float a, float b, float epsilon = std::numeric_limits<float>::epsilon());
		static inline float clip (float x, float min, float max); // hard clips x to min and max
		static inline float lerp (float t, float a, float b); // linearly interpolates between a and b based on t
		static inline float percentageBetween (float x, float a, float b); // the percentage of x between a and b clipped at 0 and 1
		static inline bool clipLine (float* xStart, float* yStart, float* xEnd, float* yEnd); // returns false if line is rejected

		IGraphics() :
			m_FB(),
			m_ColorProfile(),
			m_CurrentFont( nullptr ) {}
		virtual ~IGraphics() {}

		virtual void startFrame() = 0;
		virtual void endFrame() = 0;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
inline bool IGraphics<width, height, format, api, include3D, shaderPassDataSize>::approxEqual (float a, float b, float epsilon)
{
	return std::fabs( a - b ) < epsilon;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
inline float IGraphics<width, height, format, api, include3D, shaderPassDataSize>::clip (float x, float min, float max)
{
	// TODO revisit whether it's worth it to have this clipping branchless
	return std::clamp( x, min, max );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
inline float IGraphics<width, height, format, api, include3D, shaderPassDataSize>::lerp (float t, float a, float b)
{
	return  a + ( t * (b - a) );
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
inline float IGraphics<width, height, format, api, include3D, shaderPassDataSize>::percentageBetween (float x, float a, float b)
{
	const float percentage = ( x - a ) / ( b - a );

	return percentage;
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
inline bool IGraphics<width, height, format, api, include3D, shaderPassDataSize>::clipLine (float* xStart, float* yStart,
		float* xEnd, float* yEnd)
{
	// cache points
	float xStartClipped = *xStart;
	float yStartClipped = *yStart;
	float xEndClipped   = *xEnd;
	float yEndClipped   = *yEnd;

	// regions
	const unsigned int INSIDE = 0b0000;
	const unsigned int LEFT   = 0b0001;
	const unsigned int RIGHT  = 0b0010;
	const unsigned int BOTTOM = 0b0100;
	const unsigned int TOP    = 0b1000;

	// initialize point regions as being inside screen rect
	unsigned int point1Region = INSIDE;
	unsigned int point2Region = INSIDE;

	// determine regions of point 1
	if (xStartClipped < 0.0f)
	{
		point1Region |= LEFT;
	}
	else if (xStartClipped > 1.0f)
	{
		point1Region |= RIGHT;
	}
	if (yStartClipped < 0.0f)
	{
		point1Region |= TOP;
	}
	else if (yStartClipped > 1.0f)
	{
		point1Region |= BOTTOM;
	}

	// determine regions of point 2
	if (xEndClipped < 0.0f)
	{
		point2Region |= LEFT;
	}
	else if (xEndClipped > 1.0f)
	{
		point2Region |= RIGHT;
	}
	if (yEndClipped < 0.0f)
	{
		point2Region |= TOP;
	}
	else if (yEndClipped > 1.0f)
	{
		point2Region |= BOTTOM;
	}

	if ( (point1Region == INSIDE) && (point2Region == INSIDE) ) // if both points exist inside the screen rect
	{
		return true;
	}
	else if ( point1Region & point2Region ) // if both points exist outside the screen rect in the same region
	{
		return false;
	}
	else // if at least one point exists outside of the screen rect and points exist in different regions
	{
		float tempX1 = xStartClipped;
		float tempY1 = yStartClipped;
		float tempX2 = xEndClipped;
		float tempY2 = yEndClipped;

		if (point1Region != INSIDE)
		{
			// clip point1 based on region
			if (point1Region & TOP)
			{
				if (point1Region & RIGHT)
				{
					tempX1 = 1.0f;
					tempY1 = yStartClipped + (yEndClipped - yStartClipped) *
						(1.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY1 < 0.0f) // if the line is still outside, clip again
					{
						tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
							(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY1 = 0.0f;
					}
				}
				else if (point1Region & LEFT)
				{
					tempX1 = 0.0f;
					tempY1 = yStartClipped + (yEndClipped - yStartClipped) *
						(0.0f - xStartClipped) / (xEndClipped - xStartClipped);
					if (tempY1 < 0.0f) // if the line is still outside, clip again
					{
						tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
							(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY1 = 0.0f;
					}
				}
				else
				{
					tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
						(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
					tempY1 = 0.0f;
				}
			}
			else if (point1Region & BOTTOM)
			{
				if (point1Region & RIGHT)
				{
					tempX1 = 1.0f;
					tempY1 = yStartClipped + (yEndClipped - yStartClipped) *
						(1.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY1 > 1.0f) // if the line is still outside, clip again
					{
						tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
							(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY1 = 1.0f;
					}
				}
				else if (point1Region & LEFT)
				{
					tempX1 = 0.0f;
					tempY1 = yStartClipped + (yEndClipped - yStartClipped) *
						(0.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY1 > 1.0f) // if the line is still outside, clip again
					{
						tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
							(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY1 = 1.0f;
					}
				}
				else
				{
					tempX1 = xStartClipped + (xEndClipped - xStartClipped) *
						(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
					tempY1 = 1.0f;
				}
			}
			else if (point1Region & RIGHT)
			{
				tempY1 = yStartClipped + (yEndClipped - yStartClipped) * (1.0f - xStartClipped) / (xEndClipped - xStartClipped);

				tempX1 = 1.0f;
			}
			else if (point1Region & LEFT)
			{
				tempY1 = yStartClipped + (yEndClipped - yStartClipped) * (0.0f - xStartClipped) / (xEndClipped - xStartClipped);

				tempX1 = 0.0f;
			}
		}

		if (point2Region != INSIDE)
		{
			// clip point2 based on region
			if (point2Region & TOP)
			{
				if (point2Region & RIGHT)
				{
					tempX2 = 1.0f;
					tempY2 = yStartClipped + (yEndClipped - yStartClipped) *
						(1.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY2 < 0.0f) // if the line is still outside, clip again
					{
						tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
							(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY2 = 0.0f;
					}
				}
				else if (point2Region & LEFT)
				{
					tempX2 = 0.0f;
					tempY2 = yStartClipped + (yEndClipped - yStartClipped) *
						(0.0f - xStartClipped) / (xEndClipped - xStartClipped);
					if (tempY2 < 0.0f) // if the line is still outside, clip again
					{
						tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
							(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY2 = 0.0f;
					}
				}
				else
				{
					tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
						(0.0f - yStartClipped) / (yEndClipped - yStartClipped);
					tempY2 = 0.0f;
				}
			}
			else if (point2Region & BOTTOM)
			{
				if (point2Region & RIGHT)
				{
					tempX2 = 1.0f;
					tempY2 = yStartClipped + (yEndClipped - yStartClipped) *
						(1.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY2 > 1.0f) // if the line is still outside, clip again
					{
						tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
							(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY2 = 1.0f;
					}
				}
				else if (point2Region & LEFT)
				{
					tempX2 = 0.0f;
					tempY2 = yStartClipped + (yEndClipped - yStartClipped) *
						(0.0f - xStartClipped) / (xEndClipped - xStartClipped);

					if (tempY2 > 1.0f) // if the line is still outside, clip again
					{
						tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
							(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
						tempY2 = 1.0f;
					}
				}
				else
				{
					tempX2 = xStartClipped + (xEndClipped - xStartClipped) *
						(1.0f - yStartClipped) / (yEndClipped - yStartClipped);
					tempY2 = 1.0f;
				}
			}
			else if (point2Region & RIGHT)
			{
				tempY2 = yStartClipped + (yEndClipped - yStartClipped) * (1.0f - xStartClipped) / (xEndClipped - xStartClipped);

				tempX2 = 1.0f;
			}
			else if (point2Region & LEFT)
			{
				tempY2 = yStartClipped + (yEndClipped - yStartClipped) * (0.0f - xStartClipped) / (xEndClipped - xStartClipped);

				tempX2 = 0.0f;
			}
		}

		// if x1 is still negative or over one, the entire line is off screen
		if (tempX1 < 0.0f || tempX1 > 1.0f) return false;

		// if y1 is still negative or over one, the entire line is off screen
		if (tempY1 < 0.0f || tempX1 > 1.0f) return false;

		xStartClipped = tempX1;
		yStartClipped = tempY1;
		xEndClipped   = tempX2;
		yEndClipped   = tempY2;
	}

	// replace original points with clipped points
	*xStart = xStartClipped;
	*yStart = yStartClipped;
	*xEnd   = xEndClipped;
	*yEnd   = yEndClipped;

	return true;
}

#endif // IGRAPHICS_HPP
