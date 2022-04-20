#ifndef SURFACE_HPP
#define SURFACE_HPP

/**************************************************************************
 * The Surface class defines a virtual 'screen' which can be updated
 * by calling the draw() function. Typically, a subclass will represent
 * a screen on a device and the rest of the application will periodically
 * call the draw function to update the screen.
 *
 * NOTE: SOFTWARE_RENDERING must be defined to use software rendering.
 * This can be done in the makefile with CPP_FLAGS, ect.
**************************************************************************/

enum class CP_FORMAT;

class FrameBuffer;
class ColorProfile;
class SoftwareGraphics;
class Graphics;

class Surface
{
	public:
		Surface (unsigned int width, unsigned int height, const CP_FORMAT& format);
		virtual ~Surface();

		FrameBuffer*  getFrameBuffer();
		ColorProfile* getColorProfile();

		unsigned int  getPixelWidthInBits() const;

		virtual void draw() = 0;

	protected:
		FrameBuffer*   		m_FrameBuffer;
		ColorProfile*  		m_ColorProfile;
#ifdef SOFTWARE_RENDERING
		SoftwareGraphics* 	m_Graphics;
#else
		Graphics*      		m_Graphics; // TODO if I ever implement hardware acceleration, this should be changed
#endif
};

#endif // SURFACE_HPP
