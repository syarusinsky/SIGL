#ifndef VECTOR_HPP
#define VECTOR_HPP

/**************************************************************************
 * The Vector class defines a vector of a statically given dimensions, as well
 * as some functions that can be performed on the vector.
 *
 * Note: A Vector is a row matrix
**************************************************************************/

#define _USE_MATH_DEFINES

#include "Matrix.hpp"
#include <vector>
#include <cmath>

template <unsigned int dimensions>
class Vector : public Matrix<1, dimensions>
{
	public:
		Vector (float initVal = 0.0f);
		Vector (const std::vector<float>& initVals);
		Vector (const Matrix<1, dimensions>& matrix);
		~Vector() {}

		float& at (unsigned int pos);
		float at (unsigned int pos) const;

		// specifically for 2D, 3D, or 4D vectors
		float& x();
		float x() const;
		float& y();
		float y() const;
		// specifically for 3D or 4D vectors
		float& z();
		float z() const;
		// specifically for 4D vectors
		float& w();
		float w() const;

		Vector operator* (const Matrix<dimensions, dimensions>& matrix) const;

		float length() const;
		unsigned int numDimensions() const { return dimensions; }

		float dotProduct (const Vector<dimensions>& other) const;
		Vector crossProduct (const Vector<dimensions>& other) const;

		Vector normalize() const;

	private:
};

template <unsigned int dimensions>
Vector<dimensions>::Vector (float initVal) :
	Matrix<1, dimensions>( initVal )
{
}

template <unsigned int dimensions>
Vector<dimensions>::Vector (const std::vector<float>& initVals) :
	Matrix<1, dimensions>( 0.0f )
{
	for ( unsigned int pos = 0; pos < dimensions; pos++ )
	{
		this->m_Vals.at( pos ) = initVals[pos];
	}
}

template <unsigned int dimensions>
Vector<dimensions>::Vector (const Matrix<1, dimensions>& matrix) :
	Matrix<1, dimensions>( matrix )
{
}

template <unsigned int dimensions>
float& Vector<dimensions>::at (unsigned int pos)
{
	return this->m_Vals.at( pos );
}

template <unsigned int dimensions>
float Vector<dimensions>::at (unsigned int pos) const
{
	return this->m_Vals.at( pos );
}

template <unsigned int dimensions>
float& Vector<dimensions>::x()
{
	static_assert( dimensions == 2 || dimensions == 3 || dimensions == 4, "Vector dimensions must be 2D, 3D, or 4D to use x()." );

	return this->m_Vals.at( 0 );
}

template <unsigned int dimensions>
float Vector<dimensions>::x() const
{
	static_assert( dimensions == 2 || dimensions == 3 || dimensions == 4, "Vector dimensions must be 2D, 3D, or 4D to use x()." );

	return this->m_Vals.at( 0 );
}

template <unsigned int dimensions>
float& Vector<dimensions>::y()
{
	static_assert( dimensions == 2 || dimensions == 3 || dimensions == 4, "Vector dimensions must be 2D, 3D, or 4D to use y()." );

	return this->m_Vals.at( 1 );
}

template <unsigned int dimensions>
float Vector<dimensions>::y() const
{
	static_assert( dimensions == 2 || dimensions == 3 || dimensions == 4, "Vector dimensions must be 2D, 3D, or 4D to use y()." );

	return this->m_Vals.at( 1 );
}

template <unsigned int dimensions>
float& Vector<dimensions>::z()
{
	static_assert( dimensions == 3 || dimensions == 4, "Vector dimensions must be 3D or 4D to use z()." );

	return this->m_Vals.at( 2 );
}

template <unsigned int dimensions>
float Vector<dimensions>::z() const
{
	static_assert( dimensions == 3 || dimensions == 4, "Vector dimensions must be 3D or 4D to use z()." );

	return this->m_Vals.at( 2 );
}

template <unsigned int dimensions>
float& Vector<dimensions>::w()
{
	static_assert( dimensions == 4, "Vector dimensions must be 4D to use w()." );

	return this->m_Vals.at( 3 );
}

template <unsigned int dimensions>
float Vector<dimensions>::w() const
{
	static_assert( dimensions == 4, "Vector dimensions must be 4D to use w()." );

	return this->m_Vals.at( 3 );
}

template <unsigned int dimensions>
float Vector<dimensions>::length() const
{
	float length = 0.0f;
	for ( unsigned int dimension = 0; dimension < dimensions; dimension++ )
	{
		length += this->m_Vals.at( dimension ) * this->m_Vals.at( dimension );
	}
	length = std::sqrt( length );

	return length;
}

template <unsigned int dimensions>
float Vector<dimensions>::dotProduct (const Vector<dimensions>& other) const
{
	float retVal = 0.0f;
	for ( unsigned int dimension = 0; dimension < dimensions; dimension++ )
	{
		retVal += this->m_Vals.at( dimension ) * other.m_Vals.at( dimension );
	}

	return retVal;
}

template <unsigned int dimensions>
Vector<dimensions> Vector<dimensions>::crossProduct (const Vector<dimensions>& other) const
{
	static_assert( dimensions == 4, "Vector must be 4D to take cross product, at least in my dumb library" );

	Vector<dimensions> crossProduct;
	crossProduct.m_Vals.at( 0 ) = ( this->m_Vals.at( 1 ) * other.m_Vals.at( 2 ) ) - ( this->m_Vals.at( 2 ) * other.m_Vals.at( 1 ) );
	crossProduct.m_Vals.at( 1 ) = ( this->m_Vals.at( 2 ) * other.m_Vals.at( 0 ) ) - ( this->m_Vals.at( 0 ) * other.m_Vals.at( 2 ) );
	crossProduct.m_Vals.at( 2 ) = ( this->m_Vals.at( 0 ) * other.m_Vals.at( 1 ) ) - ( this->m_Vals.at( 1 ) * other.m_Vals.at( 0 ) );
	crossProduct.m_Vals.at( 3 ) = 1.0f;

	return crossProduct;
}

template <unsigned int dimensions>
Vector<dimensions> Vector<dimensions>::normalize() const
{
	const float oneOverLength = 1.0f / this->length();

	Vector<dimensions> copy = *this;

	for ( unsigned int dimension = 0; dimension < dimensions; dimension++ )
	{
		copy.m_Vals.at( dimension ) *= oneOverLength;
	}

	return copy;
}

template <unsigned int dimensions>
Vector<dimensions> Vector<dimensions>::operator* (const Matrix<dimensions, dimensions>& matrix) const
{
	Vector<dimensions> outVec;

	for ( unsigned int column = 0; column < dimensions; column++ )
	{
		for ( unsigned int row = 0; row < dimensions; row++ )
		{
			outVec.m_Vals.at( column ) += ( this->m_Vals.at( row ) * matrix.at( row, column ) );
		}
	}

	return outVec;
}

#endif // VECTOR_HPP
