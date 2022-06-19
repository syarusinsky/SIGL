#ifndef MATRIX_HPP
#define MATRIX_HPP

/**************************************************************************
 * The Matrix class defines a matrix of any given row/column size as well
 * as functions that can be performed on this matrix.
**************************************************************************/

#include <cstring>

template <unsigned int rows, unsigned int columns>
class Matrix
{
	public:
		Matrix (float initVal = 0.0f);
		~Matrix() {}

		Matrix& operator= (const Matrix<rows, columns>& other);

		float& at (unsigned int row, unsigned int column);
		float at (unsigned int row, unsigned int column) const;

		Matrix operator+ (float scalar) const;
		Matrix& operator+= (float scalar);
		Matrix operator- (float scalar) const;
		Matrix& operator-= (float scalar);
		Matrix operator* (float scalar) const;
		Matrix& operator*= (float scalar);

		Matrix operator+ (const Matrix<rows, columns>& other) const;
		Matrix& operator+= (const Matrix<rows, columns>& other);
		Matrix operator- (const Matrix<rows, columns>& other) const;
		Matrix& operator-= (const Matrix<rows, columns>& other);
		Matrix operator* (const Matrix<rows, columns>& other) const;
		Matrix& operator*= (const Matrix<rows, columns>& other);

		unsigned int numRows() const { return rows; }
		unsigned int numColumns() const { return columns; }

	protected:
		float 		m_Vals[rows][columns];
};

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>::Matrix (float initVal) :
	m_Vals{ {initVal} }
{
	static_assert( rows > 0 && columns > 0, "Matrices must have at least one row and at least one column" );
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>& Matrix<rows, columns>::operator= (const Matrix<rows, columns>& other)
{
	std::memcpy( m_Vals, other.m_Vals, sizeof(float) * rows * columns );

	return *this;
}

template <unsigned int rows, unsigned int columns>
float& Matrix<rows, columns>::at (unsigned int row, unsigned int column)
{
	return m_Vals[row][column];
}

template <unsigned int rows, unsigned int columns>
float Matrix<rows, columns>::at (unsigned int row, unsigned int column) const
{
	return m_Vals[row][column];
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::operator+ (float scalar) const
{
	Matrix copy = *this;

	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			copy.m_Vals[row][column] += scalar;
		}
	}

	return copy;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>& Matrix<rows, columns>::operator+= (float scalar)
{
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			m_Vals[row][column] += scalar;
		}
	}

	return *this;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::operator- (float scalar) const
{
	Matrix copy = *this;

	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			copy.m_Vals[row][column] -= scalar;
		}
	}

	return copy;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>& Matrix<rows, columns>::operator-= (float scalar)
{
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			m_Vals[row][column] -= scalar;
		}
	}

	return *this;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::operator* (float scalar) const
{
	Matrix copy = *this;

	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			copy.m_Vals[row][column] *= scalar;
		}
	}

	return copy;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>& Matrix<rows, columns>::operator*= (float scalar)
{
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			m_Vals[row][column] *= scalar;
		}
	}

	return *this;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::operator+ (const Matrix<rows,columns>& other) const
{
	Matrix copy = *this;

	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			copy.m_Vals[row][column] += other.m_Vals[row][column];
		}
	}

	return copy;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>& Matrix<rows, columns>::operator+= (const Matrix<rows, columns>& other)
{
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			m_Vals[row][column] += other.m_Vals[row][column];
		}
	}

	return *this;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::operator- (const Matrix<rows,columns>& other) const
{
	Matrix copy = *this;

	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			copy.m_Vals[row][column] -= other.m_Vals[row][column];
		}
	}

	return copy;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>& Matrix<rows, columns>::operator-= (const Matrix<rows, columns>& other)
{
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			m_Vals[row][column] -= other.m_Vals[row][column];
		}
	}

	return *this;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::operator* (const Matrix<rows, columns>& other) const
{
	Matrix<rows, columns> retMatrix( 0.0f );

	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int column = 0; column < columns; column++ )
		{
			for ( unsigned int common = 0; common < columns; common++ )
			{
				retMatrix.m_Vals[row][column] += m_Vals[row][common] * other.m_Vals[common][column];
			}
		}
	}

	return retMatrix;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>& Matrix<rows, columns>::operator*= (const Matrix<rows, columns>& other)
{
	Matrix<rows, columns> copyMatrix = *this * other;
	*this = copyMatrix;

	return *this;
}

#endif // MATRIX_HPP
