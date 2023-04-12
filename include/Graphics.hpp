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

#include "SoftwareGraphics.hpp"
#include "OpenGlGraphics.hpp"

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
class Graphics : public std::conditional<(api == RENDER_API::SOFTWARE), SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>,
				OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>>::type
{
	// only a surface should be able to construct
	template<RENDER_API rAPI, unsigned int w, unsigned int h, CP_FORMAT f, unsigned int nT, bool i3D, unsigned int sPDS> friend class SurfaceThreaded;
	template<RENDER_API rAPI, unsigned int w, unsigned int h, CP_FORMAT f, bool i3D, unsigned int sPDS> friend class SurfaceSingleCore;

	protected:
		Graphics();
		virtual ~Graphics() override;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
Graphics<width, height, format, api, include3D, shaderPassDataSize>::Graphics() :
	std::conditional<(api == RENDER_API::SOFTWARE), SoftwareGraphics<width, height, format, api, include3D, shaderPassDataSize>,
			OpenGlGraphics<width, height, format, api, include3D, shaderPassDataSize>>::type()
{
}

template <unsigned int width, unsigned int height, CP_FORMAT format, RENDER_API api, bool include3D, unsigned int shaderPassDataSize>
Graphics<width, height, format, api, include3D, shaderPassDataSize>::~Graphics()
{
}

#endif // GRAPHICS_HPP
