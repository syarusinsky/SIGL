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
#include <thread>

#ifdef SOFTWARE_RENDERING
#include "SoftwareGraphics.hpp"
#define GRAPHICS SoftwareGraphics<width, height, format, bufferSize>
#else
#include "SoftwareGraphics.hpp" // TODO if I ever implement hardware acceleration, this should be changed
#define GRAPHICS Graphics<width, height, format, bufferSize>
#endif // SOFTWARE_RENDERING

enum class CP_FORMAT;

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
class SurfaceBase
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
		GRAPHICS* 	m_Graphics;

		virtual void draw(GRAPHICS* graphics) = 0;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
class SurfaceThreaded : public SurfaceBase<width, height, format, bufferSize>
{
	public:
		SurfaceThreaded() :
			m_GraphicsBuffer { nullptr },
			m_GraphicsBufferReadIncr( 0 ),
			m_GraphicsBufferWriteIncr( bufferSize - 1 ),
			m_GraphicsThreadsDone{ false },
			m_GraphicsRead( m_GraphicsBuffer[0] )
		{
			SurfaceBase<width, height, format, bufferSize>::m_Graphics = m_GraphicsBuffer[bufferSize - 1];
			for ( unsigned int bufferNum = 0; bufferNum < bufferSize; bufferNum++ )
			{
				m_GraphicsBuffer[bufferNum] = new GRAPHICS();
			}
		}

		FrameBuffer<width, height, format>& advanceFrameBuffer()
		{
			updateGraphicsRead();
			FrameBuffer<width, height, format>& fb = m_GraphicsRead->getFrameBuffer();
			return fb;
		}

		void setFont (Font* font) override
		{
			for ( unsigned int bufferNum = 0; bufferNum < bufferSize; bufferNum++ )
			{
				m_GraphicsBuffer[bufferNum]->setFont( font );
			}
		}

		bool render() // returns false if there is no more free frames available for threading
		{
			// if we approach the read pointer, don't continue rendering
			if ( (m_GraphicsBufferWriteIncr < m_GraphicsBufferReadIncr && m_GraphicsBufferWriteIncr == m_GraphicsBufferReadIncr - 1)
					|| (m_GraphicsBufferWriteIncr > m_GraphicsBufferReadIncr && m_GraphicsBufferWriteIncr == bufferSize - 1
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
			m_GraphicsThreads[m_GraphicsBufferWriteIncr] = std::thread(
					&SurfaceThreaded::drawWrapper, this, SurfaceBase<width, height, format, bufferSize>::m_Graphics,
					m_GraphicsBufferWriteIncr );

			return true;
		}

	protected:
		virtual void draw(GRAPHICS* graphics) override = 0;

	private:
		std::array<GRAPHICS*, bufferSize> 	m_GraphicsBuffer;
		unsigned int 				m_GraphicsBufferReadIncr;
		unsigned int 				m_GraphicsBufferWriteIncr;
		std::array<std::thread, bufferSize>	m_GraphicsThreads;
		std::array<bool, bufferSize> 		m_GraphicsThreadsDone;
		using 		SurfaceBase<width, height, format, bufferSize>::m_Graphics;
		GRAPHICS* 	m_GraphicsRead;

		void drawWrapper(GRAPHICS* graphics, unsigned int bufferNum)
		{
			this->draw( graphics );
			m_GraphicsThreadsDone[bufferNum] = true;
		}

		void updateGraphicsRead()
		{
			m_GraphicsBufferReadIncr = ( m_GraphicsBufferReadIncr + 1 ) % bufferSize;
			m_GraphicsRead = m_GraphicsBuffer[m_GraphicsBufferReadIncr];
		}

		void advanceGraphicsWritePointer()
		{
			m_GraphicsBufferWriteIncr = ( m_GraphicsBufferWriteIncr + 1 ) % bufferSize;

			SurfaceBase<width, height, format, bufferSize>::m_Graphics = m_GraphicsBuffer[m_GraphicsBufferWriteIncr];
		}
};

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
class SurfaceSingleCore : public SurfaceBase<width, height, format, bufferSize>
{
	public:
		SurfaceSingleCore()
		{
			SurfaceBase<width, height, format, bufferSize>::m_Graphics = new GRAPHICS();
		}

		FrameBuffer<width, height, format>& advanceFrameBuffer()
		{
			return SurfaceBase<width, height, format, bufferSize>::m_Graphics->getFrameBuffer();
		}

		void setFont (Font* font) override
		{
			SurfaceBase<width, height, format, bufferSize>::m_Graphics->setFont( font );
		}

		bool render()
		{
			draw( SurfaceBase<width, height, format, bufferSize>::m_Graphics );
			return false;
		}

	protected:
		virtual void draw(GRAPHICS* graphics) = 0;

	private:
		using 	SurfaceBase<width, height, format, bufferSize>::m_Graphics;
};

template <unsigned int width, unsigned int height, CP_FORMAT format, unsigned int bufferSize>
class Surface : public std::conditional<(bufferSize > 1), SurfaceThreaded<width, height, format, bufferSize>,
					SurfaceSingleCore<width, height, format, bufferSize>>::type
{
	public:
		Surface() :
			std::conditional<(bufferSize > 1), SurfaceThreaded<width, height, format, bufferSize>,
					SurfaceSingleCore<width, height, format, bufferSize>>::type()
		{
		}
		virtual ~Surface() {}

	protected:
		virtual void draw(GRAPHICS* graphics) = 0;
};

#undef GRAPHICS

#endif // SURFACE_HPP
