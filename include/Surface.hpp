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

#define _USE_MATH_DEFINES

#include "ColorProfile.hpp"
#include "FrameBuffer.hpp"
#include <thread>

#include "SoftwareGraphics.hpp"
#include "OpenGlGraphics.hpp"

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
class SurfaceBaseSoftwareRendering
{
	protected:
		virtual void draw(SoftwareGraphics<width, height, format, include3D, shaderPassDataSize>* graphics) = 0;

		SoftwareGraphics<width, height, format, include3D, shaderPassDataSize>* m_Graphics;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
class SurfaceBaseOpenGl
{
	protected:
		virtual void draw(OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>* graphics) = 0;

		OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>* m_Graphics;
};

template <RENDER_API api, unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
class SurfaceBase : public std::conditional<(api == RENDER_API::SOFTWARE),
				SurfaceBaseSoftwareRendering<width, height, format, include3D, shaderPassDataSize>,
				SurfaceBaseOpenGl<width, height, format, include3D, shaderPassDataSize>>::type
{
	public:
		const ColorProfile<format>& getColorProfile() const { return m_Graphics->getColorProfile(); }

		unsigned int getPixelWidthInBits() const
		{
			switch ( this->getColorProfile().getFormat() )
			{
				case CP_FORMAT::RGBA_32BIT:
					return 32;
				case CP_FORMAT::RGB_24BIT:
					return 24;
				case CP_FORMAT::MONOCHROME_1BIT:
					return 1;
				default:
					return 0;
			}
		}

		virtual void setFont (Font* font) = 0;

		unsigned int getWidth() { return width; }
		unsigned int getHeight() { return height; }

	protected:
		using std::conditional<(api == RENDER_API::SOFTWARE),
			SurfaceBaseSoftwareRendering<width, height, format, include3D, shaderPassDataSize>,
			SurfaceBaseOpenGl<width, height, format, include3D, shaderPassDataSize>>::type::m_Graphics;
};

template <RENDER_API api, unsigned int width, unsigned int height, CP_FORMAT format, unsigned int numRenderThreads, bool include3D,
		unsigned int shaderPassDataSize>
class SurfaceThreaded : public SurfaceBase<api, width, height, format, include3D, shaderPassDataSize>
{
	using GRAPHICS = typename std::conditional<(api == RENDER_API::SOFTWARE),
						SoftwareGraphics<width, height, format, include3D, shaderPassDataSize>,
						OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>>::type;

	public:
		SurfaceThreaded() :
			m_GraphicsBuffer { nullptr },
			m_GraphicsBufferReadIncr( 0 ),
			m_GraphicsBufferWriteIncr( numRenderThreads - 1 ),
			m_GraphicsThreadsDone{ true },
			m_GraphicsRead( nullptr )
		{
			m_Graphics = m_GraphicsBuffer[numRenderThreads - 1];
			for ( unsigned int bufferNum = 0; bufferNum < numRenderThreads; bufferNum++ )
			{
				m_GraphicsBuffer[bufferNum] = new GRAPHICS();
				m_GraphicsThreadsDone[bufferNum] = true;
			}

			m_GraphicsRead = m_GraphicsBuffer[0];
		}

		~SurfaceThreaded()
		{
			for ( unsigned int bufferNum = 0; bufferNum < numRenderThreads; bufferNum++ )
			{
				if ( m_GraphicsThreads[bufferNum].joinable() )
				{
					m_GraphicsThreads[bufferNum].join();
				}
				delete m_GraphicsBuffer[bufferNum];
			}
		}

		FRAMEBUFFER& advanceFrameBuffer()
		{
			updateGraphicsRead();
			FRAMEBUFFER& fb = m_GraphicsRead->getFrameBuffer();
			return fb;
		}

		void setFont (Font* font) override
		{
			for ( unsigned int bufferNum = 0; bufferNum < numRenderThreads; bufferNum++ )
			{
				m_GraphicsBuffer[bufferNum]->setFont( font );
			}
		}

		bool render() // returns false if there is no more free frames available for threading
		{
			// if we approach the read pointer, don't continue rendering
			if ( (m_GraphicsBufferWriteIncr < m_GraphicsBufferReadIncr && m_GraphicsBufferWriteIncr == m_GraphicsBufferReadIncr - 1)
					|| (m_GraphicsBufferWriteIncr > m_GraphicsBufferReadIncr && m_GraphicsBufferWriteIncr == numRenderThreads - 1
						&& m_GraphicsBufferReadIncr == 0) )
			{
				return false;
			}

			advanceGraphicsWritePointer();

			// ensure the next frame in the buffer is drawn
			if ( m_GraphicsThreads[m_GraphicsBufferWriteIncr].joinable() )
			{
				m_GraphicsThreads[m_GraphicsBufferWriteIncr].join();
			}

			// launch a new thread to render the frame
			m_GraphicsThreadsDone[m_GraphicsBufferWriteIncr] = false;
			m_GraphicsThreads[m_GraphicsBufferWriteIncr] = std::thread( &SurfaceThreaded::drawWrapper, this, m_Graphics,
											m_GraphicsBufferWriteIncr );

			return true;
		}

	private:
		std::array<GRAPHICS*, numRenderThreads> 	m_GraphicsBuffer;
		unsigned int 					m_GraphicsBufferReadIncr;
		unsigned int 					m_GraphicsBufferWriteIncr;
		std::array<std::thread, numRenderThreads>	m_GraphicsThreads;
		std::array<volatile bool, numRenderThreads> 	m_GraphicsThreadsDone;

		using 		SurfaceBase<api, width, height, format, include3D, shaderPassDataSize>::m_Graphics;
		GRAPHICS* 	m_GraphicsRead;

		void drawWrapper(GRAPHICS* graphics, unsigned int bufferNum)
		{
			if constexpr ( include3D )
			{
				graphics->clearDepthBuffer();
			}
			this->draw( graphics );
			m_GraphicsThreadsDone[bufferNum] = true;
		}

		void updateGraphicsRead()
		{
			// only advance if the frame is done rendering
			unsigned int tempGraphicsBufferReadIncr = ( m_GraphicsBufferReadIncr + 1 ) % numRenderThreads;
			while ( ! m_GraphicsThreadsDone[tempGraphicsBufferReadIncr] ) {}

			m_GraphicsBufferReadIncr = tempGraphicsBufferReadIncr;
			m_GraphicsRead = m_GraphicsBuffer[m_GraphicsBufferReadIncr];
		}

		void advanceGraphicsWritePointer()
		{
			m_GraphicsBufferWriteIncr = ( m_GraphicsBufferWriteIncr + 1 ) % numRenderThreads;

			m_Graphics = m_GraphicsBuffer[m_GraphicsBufferWriteIncr];
		}
};

template <RENDER_API api, unsigned int width, unsigned int height, CP_FORMAT format, bool include3D, unsigned int shaderPassDataSize>
class SurfaceSingleCore : public SurfaceBase<api, width, height, format, include3D, shaderPassDataSize>
{
	using GRAPHICS = typename std::conditional<(api == RENDER_API::SOFTWARE),
						SoftwareGraphics<width, height, format, include3D, shaderPassDataSize>,
						OpenGlGraphics<width, height, format, include3D, shaderPassDataSize>>::type;

	public:
		SurfaceSingleCore()
		{
			m_Graphics = new GRAPHICS();
		}

		~SurfaceSingleCore()
		{
			delete m_Graphics;
		}

		FRAMEBUFFER& advanceFrameBuffer()
		{
			return m_Graphics->getFrameBuffer();
		}

		void setFont (Font* font) override
		{
			m_Graphics->setFont( font );
		}

		bool render()
		{
			if constexpr ( include3D )
			{
				m_Graphics->clearDepthBuffer();
			}
			this->draw( m_Graphics );
			return false;
		}

	private:
		using 		SurfaceBase<api, width, height, format, include3D, shaderPassDataSize>::m_Graphics;
};

template <RENDER_API api, unsigned int width, unsigned int height, CP_FORMAT format, unsigned int numRenderThreads, bool include3D,
		unsigned int shaderPassDataSize>
class Surface : public std::conditional<(numRenderThreads > 1),
				SurfaceThreaded<api, width, height, format, numRenderThreads, include3D, shaderPassDataSize>,
				SurfaceSingleCore<api, width, height, format, include3D, shaderPassDataSize>>::type
{
	public:
		Surface() :
			std::conditional<(numRenderThreads > 1),
				SurfaceThreaded<api, width, height, format, numRenderThreads, include3D, shaderPassDataSize>,
				SurfaceSingleCore<api, width, height, format, include3D, shaderPassDataSize>>::type()
		{
		}
		virtual ~Surface() {}

		constexpr unsigned int getWidth() { return width; }
		constexpr unsigned int getHeight() { return height; }
};

#endif // SURFACE_HPP
