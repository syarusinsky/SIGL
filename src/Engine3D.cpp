#include "Engine3D.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include "SLOGE.hpp"


static inline float lerp (float t, float a, float b)
{
	return  a + ( t * (b - a) );
}

Vertex Vertex::lerp (const Vertex& other, float lerpAmount)
{
	Vertex newVertex;

	// lerp position and normals
	for ( unsigned int vecIndex = 0; vecIndex < 4; vecIndex++ )
	{
		newVertex.vec.at( vecIndex ) = ::lerp( lerpAmount, vec.at(vecIndex), other.vec.at(vecIndex) );
		newVertex.normal.at( vecIndex ) = ::lerp( lerpAmount, normal.at(vecIndex), other.normal.at(vecIndex) );
	}

	// lerp tex coords
	for ( unsigned int vecIndex = 0; vecIndex < 2; vecIndex++ )
	{
		newVertex.texCoords.at( vecIndex ) = ::lerp( lerpAmount, texCoords.at(vecIndex), other.texCoords.at(vecIndex) );
	}

	return newVertex;
}

bool Vertex::isInsideView()
{
	return std::abs( vec.x() ) <= std::abs( vec.w() )
		&& std::abs( vec.y() ) <= std::abs( vec.w() )
		&& std::abs( vec.z() ) <= std::abs( vec.w() );
}

void Face::printFace()
{
	SLOG::log( LogLevels::INFO, LogMethodsE::INFO, std::to_string(vertices[0].vec.x()) + ", " + std::to_string(vertices[0].vec.y()) );
	SLOG::log( LogLevels::INFO, LogMethodsE::INFO, std::to_string(vertices[1].vec.x()) + ", " + std::to_string(vertices[1].vec.y()) );
	SLOG::log( LogLevels::INFO, LogMethodsE::INFO, std::to_string(vertices[2].vec.x()) + ", " + std::to_string(vertices[2].vec.y()) );
}

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

void Mesh::applyTransformations()
{
	for ( unsigned int index = 0; index < faces.size(); index++ )
	{
		faces[index] = transformedFace( index );
	}

	transformMat = generateIdentityMatrix();
}

Face Mesh::transformedFace (unsigned int index)
{
	Face transformedFace = faces[index];
	transformedFace.vertices[0].vec = transformedFace.vertices[0].vec * transformMat;
	transformedFace.vertices[1].vec = transformedFace.vertices[1].vec * transformMat;
	transformedFace.vertices[2].vec = transformedFace.vertices[2].vec * transformMat;
	transformedFace.vertices[0].normal = transformedFace.vertices[0].normal * transformMat;
	transformedFace.vertices[1].normal = transformedFace.vertices[1].normal * transformMat;
	transformedFace.vertices[2].normal = transformedFace.vertices[2].normal * transformMat;

	return transformedFace;
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
	m_Left( 0.0f ),
	m_Right( 0.0f ),
	m_Bottom( 0.0f ),
	m_Top( 0.0f ),
	m_Near( 0.0f ),
	m_Far( 0.0f ),
	m_NearClip( nearClip ),
	m_FarClip( farClip ),
	m_FieldOfView( fieldOfView ),
	m_AspectRatio( aspectRatio ),
	m_ProjectionMatrix( 0.0f ),
	m_Position( 0.0f )
{
	this->generatePerspectiveProjectionMatrix();
}

Camera3D::Camera3D (float left, float right, float bottom, float top, float near, float far) :
	m_Left( left ),
	m_Right( right ),
	m_Bottom( bottom ),
	m_Top( top ),
	m_Near( near ),
	m_Far( far ),
	m_NearClip( 0.0f ),
	m_FarClip( 1.0f ),
	m_FieldOfView( 0.0f ),
	m_AspectRatio( 0.0 ),
	m_ProjectionMatrix( generateIdentityMatrix() ),
	m_Position( 0.0f )
{
	this->generateOrthographicProjectionMatrix();
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

void Camera3D::generatePerspectiveProjectionMatrix()
{
	constexpr float halfPiOver180 = ( M_PI / 180.0f ) * 0.5f;
	const float tanHalfFov = tanf(m_FieldOfView * halfPiOver180);

	m_ProjectionMatrix = Matrix<4, 4>( 0.0f ); // reset the projection matrix
	m_ProjectionMatrix.at( 0, 0 ) = 1.0f / ( tanHalfFov * m_AspectRatio );
	m_ProjectionMatrix.at( 1, 1 ) = 1.0f / tanHalfFov;
	m_ProjectionMatrix.at( 2, 2 ) = m_FarClip / ( m_FarClip - m_NearClip );
	m_ProjectionMatrix.at( 3, 2 ) = ( -m_FarClip * m_NearClip ) / ( m_FarClip - m_NearClip );
	m_ProjectionMatrix.at( 2, 3 ) = 1.0f;
}

void Camera3D::generateOrthographicProjectionMatrix()
{
	const float width = m_Right - m_Left;
	const float height = m_Top - m_Bottom;
	const float depth = m_Far - m_Near;

	// inverted
	m_ProjectionMatrix = Matrix<4, 4>( 0.0f ); // reset the projection matrix
	m_ProjectionMatrix.at( 0, 0 ) = 2.0f / width;
	m_ProjectionMatrix.at( 3, 0 ) = -( m_Right + m_Left ) / width;
	m_ProjectionMatrix.at( 1, 1 ) = 2.0f / height;
	m_ProjectionMatrix.at( 3, 1 ) = -( m_Top + m_Bottom ) / height;
	m_ProjectionMatrix.at( 2, 2 ) = -2.0f / depth;
	m_ProjectionMatrix.at( 3, 2 ) = -( m_Far + m_Near ) / depth;
	m_ProjectionMatrix.at( 3, 3 ) = 1.0f;
}

Vector<4> Camera3D::multiplyByCameraMatrix (const Vector<4>& vector)
{
	return vector * m_ProjectionMatrix;
}

Vector<4> Camera3D::perspectiveDivide (const Vector<4>& vector)
{
	Vector<4> outVec = vector;

	if ( outVec.w() != 0.0f )
	{
		outVec.x() = outVec.x() / outVec.w();
		outVec.y() = outVec.y() / outVec.w();
		outVec.z() = outVec.z() / outVec.w();
	}

	return outVec;
}

void Camera3D::multiplyByCameraMatrix (Face& face)
{
	face.vertices[0].vec = multiplyByCameraMatrix( face.vertices[0].vec );
	face.vertices[1].vec = multiplyByCameraMatrix( face.vertices[1].vec );
	face.vertices[2].vec = multiplyByCameraMatrix( face.vertices[2].vec );
}

void Camera3D::perspectiveDivide (Face& face)
{
	face.vertices[0].vec = perspectiveDivide( face.vertices[0].vec );
	face.vertices[1].vec = perspectiveDivide( face.vertices[1].vec );
	face.vertices[2].vec = perspectiveDivide( face.vertices[2].vec );
}

void Camera3D::scaleXYToZeroToOne (Face& face) const
{
	face.vertices[0].vec.x() = ( face.vertices[0].vec.x() + 1.0f ) * 0.5f; face.vertices[0].vec.y() = ( face.vertices[0].vec.y() + 1.0f ) * 0.5f;
	face.vertices[1].vec.x() = ( face.vertices[1].vec.x() + 1.0f ) * 0.5f; face.vertices[1].vec.y() = ( face.vertices[1].vec.y() + 1.0f ) * 0.5f;
	face.vertices[2].vec.x() = ( face.vertices[2].vec.x() + 1.0f ) * 0.5f; face.vertices[2].vec.y() = ( face.vertices[2].vec.y() + 1.0f ) * 0.5f;
}
