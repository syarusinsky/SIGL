#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "FrameBuffer.hpp"
#include "ColorProfile.hpp"
#include "SoftwareGraphics.hpp"

class Surface
{
	public:
		Surface (unsigned int width, unsigned int height, const CP_FORMAT& format);
		~Surface();

		FrameBuffer*  getFrameBuffer();
		ColorProfile* getColorProfile();

		unsigned int  getPixelWidthInBits();

	protected:
		FrameBuffer  	 m_FrameBuffer;
		ColorProfile*	 m_ColorProfile;
		SoftwareGraphics m_Graphics;
};

#endif // SURFACE_HPP
