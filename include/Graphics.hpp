#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

/*************************************************************************
 * The Graphics class defines several helper methods to draw graphical
 * elements to a frame buffer. The Graphics constructor is protected (as
 * should be the subclasses that define the implementation), so that
 * only a Surface class can construct them. This is so that typically all
 * drawing code will be done inside a surface. The class is abstract,
 * so that both a software rendering version and a hardware accelerated
 * version can be implemented.
*************************************************************************/

#define _USE_MATH_DEFINES

#include "FrameBuffer.hpp"
#include "ColorProfile.hpp"
#include <string>
#include <math.h>

class Font;
class Sprite;
class Camera3D;
class Texture;
struct Face;

template <unsigned int width, unsigned int height, CP_FORMAT format>
class Graphics
{
	public:
		inline unsigned int convertXPercentageToUInt (float x) { return x * (width  - 1); }
		inline unsigned int convertYPercentageToUInt (float y) { return y * (height - 1); }

		virtual void setColor (float r, float g, float b) = 0;
		virtual void setColor (bool val) = 0;

		virtual void setFont (Font* font) = 0;

		virtual void fill() = 0;
		virtual void drawLine (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawBox (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd) = 0;
		virtual void drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void drawTriangleShaded (Face& face, const Camera3D& camera) = 0;
		virtual void drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
		virtual void drawCircle (float originX, float originY, float radius) = 0;
		virtual void drawCircleFilled (float originX, float originY, float radius) = 0;
		virtual void drawText (float xStart, float yStart, const char* text, float scaleFactor) = 0;
		virtual void drawSprite (float xStart, float yStart, Sprite& sprite) = 0;

		// TODO this is probably not the right way to handle shaders, for testing purposes only now
		void setVertexShader (void (*vShader)(Face& face)) { vertexShader = vShader; }
		template <unsigned int texWidth, unsigned int texHeight, CP_FORMAT texFormat>
		void setFragmentShader (void (*fShader)(Color& color, Face& face, Texture<texWidth, texHeight, texFormat>& tex, float v1Cur,
							float v2Cur, float v3Cur, float texCoordX, float texCoordY)) { fragmentShader = fShader; }

		inline static float distance (float x1, float y1, float x2, float y2) { return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2)); }

		// TODO remove this after testing
		virtual void testTexture (Texture& texture) = 0;

	protected:
		FrameBuffer<width, height, format> 	m_FB;
		ColorProfile<format>& 			m_ColorProfile;
		Font* 					m_CurrentFont;
		const unsigned int 			m_NumPixels;

		// TODO this is probably not the right way to handle shaders, for testing purposes only now
		// shaders (default vertex does nothing, default fragment is a rgb gradient)
		void (*vertexShader)(Face& face) = [](Face& face) { return; };
		void (*fragmentShader)(Color& color, Face& face, Texture* tex, float v1Cur, float v2Cur, float v3Cur, float texCoordX,
					float texCoordY) =
			[](Color& color, Face& face, Texture* tex, float v1Cur, float v2Cur, float v3Cur, float texCoordX, float texCoordY)
			{
				color.m_R = v1Cur;
				color.m_G = v2Cur;
				color.m_B = v3Cur;
				color.m_A = 1.0f;
				color.m_M = false;
				color.m_IsMonochrome = false;
				color.m_HasAlpha = false;
			};

		// helpers
		static inline bool clipLine (float* xStart, float* yStart, float* xEnd, float* yEnd); // returns false if line is rejected

		Graphics (FrameBuffer<width, height, format> frameBuffer) :
			m_FB( frameBuffer ),
			m_ColorProfile( m_FB.getColorProfile() ),
			m_CurrentFont( nullptr ),
			m_NumPixels( m_FB.getNumPixels() ) {}
		virtual ~Graphics() {}

};

template <unsigned int width, unsigned int height, CP_FORMAT format>
inline bool Graphics<width, height, format>::clipLine (float* xStart, float* yStart, float* xEnd, float* yEnd)
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

#endif // GRAPHICS_HPP
