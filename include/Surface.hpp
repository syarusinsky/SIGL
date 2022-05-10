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

#include "ColorProfile.hpp"
#include "FrameBuffer.hpp"

#ifdef SOFTWARE_RENDERING
#include "SoftwareGraphics.hpp"
#else
#include "SoftwareGraphics.hpp" // TODO if I ever implement hardware acceleration, this should be changed
#endif // SOFTWARE_RENDERING

enum class CP_FORMAT;

template <unsigned int width, unsigned int height, CP_FORMAT format>
class Surface
{
	public:
		Surface() :
			m_FrameBuffer(),
			m_ColorProfile(),
#ifdef SOFTWARE_RENDERING
			m_Graphics( new SoftwareGraphics<width, height, format>() )
#else
			m_Graphics( new SoftwareGraphics<width, height, format>() ) // TODO if I ever implement hardware acceleration, this should be changed
#endif // SOFTWARE_RENDERING
		{
		}
		virtual ~Surface() {}

		FrameBuffer<width, height, format>& getFrameBuffer() { return m_FrameBuffer; }
		ColorProfile<format>& getColorProfile() { return m_ColorProfile; }

		unsigned int getPixelWidthInBits() const
		{
			switch ( m_ColorProfile.getFormat() )
			{
				case CP_FORMAT::RGB_24BIT:
					return 24;
				case CP_FORMAT::MONOCHROME_1BIT:
					return 1;
				default:
					return 0;
			}
		}

		virtual void draw() = 0;

	protected:
		FrameBuffer<width, height, format>   		m_FrameBuffer;
		ColorProfile<format>  				m_ColorProfile;
#ifdef SOFTWARE_RENDERING
		SoftwareGraphics<width, height, format>* 	m_Graphics;
#else
		Graphics<width, height, format>*      		m_Graphics; // TODO if I ever implement hardware acceleration, this should be changed
#endif
};

#endif // SURFACE_HPP
