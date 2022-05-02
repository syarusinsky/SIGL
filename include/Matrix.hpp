#ifndef MATRIX_HPP
#define MATRIX_HPP

/**************************************************************************
 * The Matrix class defines a matrix of any given row/column size as well
 * as functions that can be performed on this matrix.
**************************************************************************/

template <unsigned int rows, unsigned int columns>
class Matrix
{
	public:
		Matrix (float initVal = 0.0f);
		~Matrix() {}

		float& at (unsigned int row, unsigned int column);
		float at (unsigned int row, unsigned int column) const;

		Matrix operator+ (float scalar) const;
		Matrix operator+= (float scalar);
		Matrix operator- (float scalar) const;
		Matrix operator-= (float scalar);
		Matrix operator* (float scalar) const;
		Matrix operator*= (float scalar);

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
Matrix<rows, columns> Matrix<rows, columns>::operator+= (float scalar)
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
Matrix<rows, columns> Matrix<rows, columns>::operator-= (float scalar)
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
Matrix<rows, columns> Matrix<rows, columns>::operator*= (float scalar)
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

#endif // MATRIX_HPP
