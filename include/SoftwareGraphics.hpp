#ifndef SOFTWAREGRAPHICS_HPP
#define SOFTWAREGRAPHICS_HPP

/**************************************************************************
 * The SofwareGraphics class defines functions used to render graphics
 * with CPU-based rendering, as opposed to GPU-based rendering. This is
 * useful for embedded applications where a GPU may not be available. The
 * constructor is protected, so that only a Surface object can construct
 * an instance and all drawing code can take place in the context of the
 * surface.
**************************************************************************/

#include "Graphics.hpp"

class SoftwareGraphics 	: public Graphics
{
	// only a surface should be able to construct
	friend class Surface;

	public:
		void setColor (float r, float g, float b) override;
		void setColor (bool val) override;
		void setFont (Font* font) override;

		void fill();
		void drawLine (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawBox (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawBoxFilled (float xStart, float yStart, float xEnd, float yEnd) override;
		void drawTriangle (float x1, float y1, float x2, float y2, float x3, float y3) override;
		void drawTriangleFilled (float x1, float y1, float x2, float y2, float x3, float y3) override;
		void drawQuad (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
		void drawQuadFilled (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) override;
		void drawCircle (float originX, float originY, float radius) override;
		void drawCircleFilled (float originX, float originY, float radius) override;
		void drawText (float xStart, float yStart, std::string, float scaleFactor) override;

	protected:
		bool clipLine (float* xStart, float* yStart, float* xEnd, float* yEnd); // returns false if line is rejected
		void drawCircleHelper (int originX, int originY, int x, int y, bool filled = false);

		SoftwareGraphics (FrameBuffer* frameBuffer);
		~SoftwareGraphics() override;
};

#endif // SOFTWARE_GRAPHICS_HPP
