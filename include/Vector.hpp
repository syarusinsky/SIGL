#ifndef VECTOR_HPP
#define VECTOR_HPP

/**************************************************************************
 * The Vector class defines a vector of a statically given dimensions, as well
 * as some functions that can be performed on the vector.
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

		float length() const;
		unsigned int numDimensions() const { return dimensions; }

		float dotProduct (const Vector<dimensions>& other) const;
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
		this->m_Vals[0][pos] = initVals[pos];
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
	return this->m_Vals[0][pos];
}

template <unsigned int dimensions>
float Vector<dimensions>::at (unsigned int pos) const
{
	return this->m_Vals[0][pos];
}

template <unsigned int dimensions>
float& Vector<dimensions>::x()
{
	static_assert( dimensions == 2 || dimensions == 3 || dimensions == 4, "Vector dimensions must be 2D, 3D, or 4D to use x()." );

	return this->m_Vals[0][0];
}

template <unsigned int dimensions>
float Vector<dimensions>::x() const
{
	static_assert( dimensions == 2 || dimensions == 3 || dimensions == 4, "Vector dimensions must be 2D, 3D, or 4D to use x()." );

	return this->m_Vals[0][0];
}

template <unsigned int dimensions>
float& Vector<dimensions>::y()
{
	static_assert( dimensions == 2 || dimensions == 3 || dimensions == 4, "Vector dimensions must be 2D, 3D, or 4D to use y()." );

	return this->m_Vals[0][1];
}

template <unsigned int dimensions>
float Vector<dimensions>::y() const
{
	static_assert( dimensions == 2 || dimensions == 3 || dimensions == 4, "Vector dimensions must be 2D, 3D, or 4D to use y()." );

	return this->m_Vals[0][1];
}

template <unsigned int dimensions>
float& Vector<dimensions>::z()
{
	static_assert( dimensions == 3 || dimensions == 4, "Vector dimensions must be 3D or 4D to use z()." );

	return this->m_Vals[0][2];
}

template <unsigned int dimensions>
float Vector<dimensions>::z() const
{
	static_assert( dimensions == 3 || dimensions == 4, "Vector dimensions must be 3D or 4D to use z()." );

	return this->m_Vals[0][2];
}

template <unsigned int dimensions>
float& Vector<dimensions>::w()
{
	static_assert( dimensions == 4, "Vector dimensions must be 4D to use w()." );

	return this->m_Vals[0][3];
}

template <unsigned int dimensions>
float Vector<dimensions>::w() const
{
	static_assert( dimensions == 4, "Vector dimensions must be 4D to use w()." );

	return this->m_Vals[0][3];
}

template <unsigned int dimensions>
float Vector<dimensions>::length() const
{
	float length = 0.0f;
	for ( unsigned int dimension = 0; dimension < dimensions; dimension++ )
	{
		length += this->m_Vals[0][dimension] * this->m_Vals[0][dimension];
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
		retVal += this->m_Vals[0][dimension] * other.m_Vals[0][dimension];
	}

	return retVal;
}

template <unsigned int dimensions>
Vector<dimensions> Vector<dimensions>::normalize() const
{
	float oneOverLength = 1.0f / this->length();

	Vector<dimensions> copy = *this;

	for ( unsigned int dimension = 0; dimension < dimensions; dimension++ )
	{
		copy.m_Vals[0][dimension] *= oneOverLength;
	}

	return copy;
}

#endif // VECTOR_HPP
