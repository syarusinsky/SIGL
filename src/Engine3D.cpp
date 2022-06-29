#include "Engine3D.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

void Mesh::scale (float scaleFactor)
{
	transformMat.at( 0, 0 ) *= scaleFactor;
	transformMat.at( 1, 1 ) *= scaleFactor;
	transformMat.at( 2, 2 ) *= scaleFactor;
}

void Mesh::translate (float x, float y, float z)
{
	transformMat.at( 3, 0 ) += x;
	transformMat.at( 3, 1 ) += y;
	transformMat.at( 3, 2 ) += z;
}

void Mesh::rotate (float x, float y, float z)
{
	Matrix<4, 4> rotMatrix = generateRotationMatrix( x, y , z );
	transformMat *= rotMatrix;
}

Vector<4> Face::calcFaceNormals()
{
	Vertex& vert1 = vertices[0];
	Vertex& vert2 = vertices[1];
	Vertex& vert3 = vertices[2];

	Vector<4> vec12( {vert2.vec.x() - vert1.vec.x(), vert2.vec.y() - vert1.vec.y(), vert2.vec.z() - vert1.vec.z(), 1.0f} );
	Vector<4> vec23( {vert3.vec.x() - vert2.vec.x(), vert3.vec.y() - vert2.vec.y(), vert3.vec.z() - vert2.vec.z(), 1.0f} );
	Vector<4> normal = vec12.crossProduct( vec23 );
	normal = normal.normalize();

	return normal;
}

Matrix<4, 4> generateIdentityMatrix()
{
	Matrix<4, 4> matrix;
	matrix.at( 0, 0 ) = 1.0f;
	matrix.at( 1, 1 ) = 1.0f;
	matrix.at( 2, 2 ) = 1.0f;
	matrix.at( 3, 3 ) = 1.0f;

	return matrix;
}

Matrix<4, 4> generateRotationMatrix (float xDegrees, float yDegrees, float zDegrees)
{
	// convert degrees to radians
	const float piOver180 = M_PI / 180.0f;
	float xRadians = xDegrees * piOver180;
	float yRadians = yDegrees * piOver180;
	float zRadians = zDegrees * piOver180;
	Matrix<4, 4> matrix( 0.0f );
	float cosX = cosf( xRadians );
	float cosY = cosf( yRadians );
	float cosZ = cosf( zRadians );
	float sinX = sinf( xRadians );
	float sinY = sinf( yRadians );
	float sinZ = sinf( zRadians );
	matrix.at(0, 0) = cosZ * cosY;
	matrix.at(0, 1) = ( cosZ * sinY * sinX ) - ( sinZ * cosX );
	matrix.at(0, 2) = ( cosZ * sinY * cosX ) + ( sinZ * sinX );
	matrix.at(1, 0) = sinZ * cosY;
	matrix.at(1, 1) = ( sinZ * sinY * sinX ) + ( cosZ * cosX );
	matrix.at(1, 2) = ( sinZ * sinY * cosX ) - ( cosZ * sinX );
	matrix.at(2, 0) = sinY * -1.0f;
	matrix.at(2, 1) = cosY * sinX;
	matrix.at(2, 2) = cosY * cosX;
	matrix.at(3, 3) = 1.0f;

	return matrix;
}

Camera3D::Camera3D (float nearClip, float farClip, float fieldOfView, float aspectRatio) :
	m_NearClip( nearClip ),
	m_FarClip( farClip ),
	m_FieldOfView( fieldOfView ),
	m_AspectRatio( aspectRatio ),
	m_ProjectionMatrix( 0.0f ),
	m_Position( 0.0f )
{
	this->generateProjectionMatrix();
}

float Camera3D::x() const
{
	return m_Position.x();
}

float Camera3D::y() const
{
	return m_Position.y();
}

float Camera3D::z() const
{
	return m_Position.z();
}

void Camera3D::generateProjectionMatrix()
{
	const float piOver180 = M_PI / 180.0f;
	const float tanHalfFov = tanf(m_FieldOfView * piOver180 * 0.5f);

	m_ProjectionMatrix = Matrix<4, 4>( 0.0f ); // reset the projection matrix
	m_ProjectionMatrix.at(0, 0) = 1.0f / ( tanHalfFov * m_AspectRatio );
	m_ProjectionMatrix.at(1, 1) = 1.0f / tanHalfFov;
	m_ProjectionMatrix.at(2, 2) = -( m_FarClip + m_NearClip ) / ( m_FarClip - m_NearClip );
	m_ProjectionMatrix.at(3, 2) = ( -2.0f * m_FarClip * m_NearClip ) / ( m_FarClip - m_NearClip );
	m_ProjectionMatrix.at(2, 3) = 1.0f;
}

static Vector<4> perspectiveMultiply (const Vector<4>& vector, const Matrix<4, 4>& matrix)
{
	Vector<4> outVec = vector * matrix;

	if ( outVec.w() != 0.0f )
	{
		outVec.x() = outVec.x() / outVec.w();
		outVec.y() = outVec.y() / outVec.w();
		outVec.z() = outVec.z() / outVec.w();
	}

	return outVec;
}

void Camera3D::projectFace (Face& face) const
{
	face.vertices[0].vec = perspectiveMultiply( face.vertices[0].vec, m_ProjectionMatrix );
	face.vertices[1].vec = perspectiveMultiply( face.vertices[1].vec, m_ProjectionMatrix );
	face.vertices[2].vec = perspectiveMultiply( face.vertices[2].vec, m_ProjectionMatrix );
}

void Camera3D::scaleXYToZeroToOne (Face& face) const
{
	face.vertices[0].vec.x() = ( face.vertices[0].vec.x() + 1.0f ) * 0.5f; face.vertices[0].vec.y() = ( face.vertices[0].vec.y() + 1.0f ) * 0.5f;
	face.vertices[1].vec.x() = ( face.vertices[1].vec.x() + 1.0f ) * 0.5f; face.vertices[1].vec.y() = ( face.vertices[1].vec.y() + 1.0f ) * 0.5f;
	face.vertices[2].vec.x() = ( face.vertices[2].vec.x() + 1.0f ) * 0.5f; face.vertices[2].vec.y() = ( face.vertices[2].vec.y() + 1.0f ) * 0.5f;
}
