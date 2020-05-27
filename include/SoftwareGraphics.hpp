#ifndef SOFTWAREGRAPHICS_HPP
#define SOFTWAREGRAPHICS_HPP

#include "Graphics.hpp"

class SoftwareGraphics 	: public Graphics
{
	public:
		SoftwareGraphics (FrameBuffer* frameBuffer, ColorProfile* colorProfile);
		~SoftwareGraphics() override;

		void setColor (float r, float g, float b) override; // r, g, b, and a should be between 0.0f and 1.0f
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

	private:
		bool clipLine (float* xStart, float* yStart, float* xEnd, float* yEnd); // returns false if line is rejected
		void drawCircleHelper (int originX, int originY, int x, int y, bool filled = false);
};

#endif // SOFTWARE_GRAPHICS_HPP
