#ifndef MATRIX_HPP
#define MATRIX_HPP

/**************************************************************************
 * The Matrix class defines a matrix of any given row/column size as well
 * as functions that can be performed on this matrix.
**************************************************************************/

#define _USE_MATH_DEFINES

#include <cmath>
#include <array>

template <unsigned int rows, unsigned int columns>
class Matrix
{
	public:
		Matrix (float initVal = 0.0f);
		Matrix (std::array<std::array<float, columns>, rows> vals);
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

		Matrix transpose() const;
		Matrix inverse() const;

		Matrix normalize() const;

		unsigned int numRows() const { return rows; }
		unsigned int numColumns() const { return columns; }

	protected:
		std::array<std::array<float, columns>, rows> m_Vals;

	private:
		float determinant (const std::array<std::array<float, columns>, rows>& vals, int dimensions = rows) const;
		std::array<std::array<float, columns>, rows> getCofactor(const std::array<std::array<float, columns>, rows>& vals) const;
};

template <unsigned int mat1Rows, unsigned int mat1Cols, unsigned int mat2Rows, unsigned int mat2Cols>
Matrix<mat1Rows, mat2Cols> matrixDotProduct(const Matrix<mat1Rows, mat1Cols>& mat1, const Matrix<mat2Rows, mat2Cols>& mat2);

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>::Matrix (float initVal) :
	m_Vals{ {{initVal}} }
{
	static_assert( rows > 0 && columns > 0, "Matrices must have at least one row and at least one column" );
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>::Matrix (std::array<std::array<float, columns>, rows> vals) :
	m_Vals{ vals }
{
	static_assert( rows > 0 && columns > 0, "Matrices must have at least one row and at least one column" );
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns>& Matrix<rows, columns>::operator= (const Matrix<rows, columns>& other)
{
	m_Vals = other.m_Vals;

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

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::transpose() const
{
	static_assert( rows == columns, "Rows != columns for finding transpose of a matrix!" );

	constexpr unsigned int dimensions = rows;

	Matrix<rows, columns> transpose;

	for ( unsigned int row = 0; row < dimensions; row++ )
	{
		for ( unsigned int column = 0; column < dimensions; column++ )
		{
			transpose.m_Vals[column][row] = m_Vals[row][column];
		}
	}

	return transpose;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::inverse() const
{
	static_assert( rows == columns, "Rows != columns for finding inverse of a matrix!" );

	constexpr unsigned int dimensions = rows;

	double oneOverDeterminant = 1.0f / this->determinant( m_Vals );

	Matrix inverse;
	inverse.m_Vals = this->getCofactor( m_Vals );
	inverse = inverse.transpose();

	for ( unsigned int row = 0; row < dimensions; row++ )
	{
		for ( unsigned int column = 0; column < dimensions; column++ )
		{
			inverse.m_Vals[row][column] *= oneOverDeterminant;
		}
	}

	return inverse;
}

template <unsigned int rows, unsigned int columns>
Matrix<rows, columns> Matrix<rows, columns>::normalize() const
{
	Matrix<rows, columns> copy = *this;

	if constexpr ( rows == columns )
	{
		const float determinant = this->determinant( m_Vals );

		if ( determinant != 0.0f )
		{
			const float oneOverDeterminant = 1.0f / determinant;

			for ( unsigned int row = 0; row < rows; row++ )
			{
				for ( unsigned int col = 0; col < columns; col++ )
				{
					copy.at( row, col ) *= oneOverDeterminant;
				}
			}

			return copy;
		}
	}

	// find the minimum value
	float min = 0.0f;
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int col = 0; col < columns; col++ )
		{
			min = ( copy.at(row, col) < min )  ? copy.at( row, col ) : min;
		}
	}
	// subtract the minimum value
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int col = 0; col < columns; col++ )
		{
			copy.at( row, col ) -= min;
		}
	}
	// find the maximum
	float max = 1.0f;
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int col = 0; col < columns; col++ )
		{
			max = ( copy.at(row, col) > max ) ? copy.at( row, col ) : max;
		}
	}
	// divide by maximum
	float oneOverMax = 1.0f / max;
	for ( unsigned int row = 0; row < rows; row++ )
	{
		for ( unsigned int col = 0; col < columns; col++ )
		{
			copy.at( row, col ) *= oneOverMax;
		}
	}

	return copy;
}

template <unsigned int rows, unsigned int columns>
float Matrix<rows, columns>::determinant (const std::array<std::array<float, columns>, rows>& vals, int dimensions) const
{
	static_assert( rows == columns, "Rows != columns for finding determinant of a matrix!" );

	float determinant = 0.0f;
	int p = 0; int h = 0; int k = 0; int i = 0; int j = 0;
	std::array<std::array<float, columns>, rows> temp = { 0.0f };

	if ( dimensions == 1 )
	{
		return vals[0][0];
	}
	else if ( dimensions == 2 )
	{
		determinant = ( vals[0][0] * vals[1][1] - vals[0][1] * vals[1][0] );
	}
	else
	{
		for ( p = 0; p < dimensions; p++ )
		{
			h = 0;
			k = 0;
			for ( i = 1; i < dimensions; i++ )
			{
				for ( j = 0; j < dimensions; j++ )
				{
					if ( j == p )
					{
						continue;
					}

					temp[h][k] = vals[i][j];
					k++;
					if ( k == dimensions - 1 )
					{
						h++;
						k = 0;
					}
				}
			}

			determinant = determinant + vals[0][p] * std::pow( -1, p ) * this->determinant( temp, dimensions-1 );
		}
	}

	return determinant;
}

template <unsigned int rows, unsigned int columns>
std::array<std::array<float, columns>, rows> Matrix<rows, columns>::getCofactor(const std::array<std::array<float, columns>, rows>& vals) const
{
	static_assert( rows == columns, "Rows != columns for finding cofactor of a matrix!" );

	constexpr unsigned int dimensions = rows;

	std::array<std::array<float, columns>, rows> solution = { 0.0f };
	std::array<std::array<float, columns>, rows> subArray = { 0.0f };

	for ( unsigned int i = 0; i < dimensions; i++ )
	{
		for ( unsigned int j = 0; j < dimensions; j++ )
		{
			int p = 0;
			for ( unsigned int x = 0; x < dimensions; x++ )
			{
				if ( x == i )
				{
					continue;
				}
				int q = 0;

				for ( unsigned int y = 0; y < dimensions; y++ )
				{
					if ( y == j )
					{
						continue;
					}

					subArray[p][q] = vals[x][y];
					q++;
				}
				p++;
			}
			solution[i][j] = std::pow( -1, i + j ) * this->determinant( subArray, dimensions - 1 );
		}
	}
	return solution;
}

template <unsigned int mat1Rows, unsigned int mat1Cols, unsigned int mat2Rows, unsigned int mat2Cols>
Matrix<mat1Rows, mat2Cols> matrixDotProduct(const Matrix<mat1Rows, mat1Cols>& mat1, const Matrix<mat2Rows, mat2Cols>& mat2)
{
	static_assert( mat1Cols == mat2Rows, "mat1Cols != mat2Rows for finding matrix dot product!" );

	Matrix<mat1Rows, mat2Cols> retMatrix;

	for ( unsigned int mat2Col = 0; mat2Col < mat2Cols; mat2Col++ )
	{
		for ( unsigned int mat1Row = 0; mat1Row < mat1Rows; mat1Row++ )
		{
			for ( unsigned int mat2Row = 0; mat2Row < mat2Rows; mat2Row++ )
			{
				retMatrix.at( mat1Row, mat2Col ) += mat1.at( mat1Row, mat2Row ) * mat2.at( mat2Row, mat2Col );
			}
		}
	}

	return retMatrix;
}

#endif // MATRIX_HPP
