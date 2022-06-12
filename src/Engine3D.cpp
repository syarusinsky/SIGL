#include "Engine3D.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

Mesh createCubeMesh()
{
	Mesh cube;
	cube.faces =
	{
		// front face
		Face{ {Vertex{Vector<4>({0.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({0.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 1.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}} },
		Face{ {Vertex{Vector<4>({0.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}, Vertex{Vector<4>({1.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 0.0f})}} },

		// right face
		Face{ {Vertex{Vector<4>({1.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 1.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}} },
		Face{ {Vertex{Vector<4>({1.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}, Vertex{Vector<4>({1.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 0.0f})}} },

		// back face
		Face{ {Vertex{Vector<4>({1.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 1.0f})}, Vertex{Vector<4>({0.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}} },
		Face{ {Vertex{Vector<4>({1.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({0.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}, Vertex{Vector<4>({0.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 0.0f})}} },

		// left face
		Face{ {Vertex{Vector<4>({0.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({0.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 1.0f})}, Vertex{Vector<4>({0.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}} },
		Face{ {Vertex{Vector<4>({0.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({0.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}, Vertex{Vector<4>({0.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 0.0f})}} },

		// top face
		Face{ {Vertex{Vector<4>({0.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({0.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 1.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}} },
		Face{ {Vertex{Vector<4>({0.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}, Vertex{Vector<4>({1.0f, 1.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 0.0f})}} },

		// bottom face
		Face{ {Vertex{Vector<4>({0.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({0.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({0.0f, 1.0f})}, Vertex{Vector<4>({1.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}} },
		Face{ {Vertex{Vector<4>({0.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({0.0f, 0.0f})}, Vertex{Vector<4>({1.0f, 0.0f, 0.0f, 1.0f}), {}, Vector<2>({1.0f, 1.0f})}, Vertex{Vector<4>({1.0f, 0.0f, 1.0f, 1.0f}), {}, Vector<2>({1.0f, 0.0f})}} }
	};

	for ( Face& face : cube.faces )
	{
		// translate to origin for rotation
		face.vertices[0].vec -= 0.5f;
		face.vertices[1].vec -= 0.5f;
		face.vertices[2].vec -= 0.5f;
	}

	return cube;
}

Vector<4> crossProductVec4D (const Vector<4>& vec1, const Vector<4>& vec2, const Vector<4>& vec3)
{
	Vector<3> vec12( {vec2.x() - vec1.x(), vec2.y() - vec1.y(), vec2.z() - vec1.z()} );
	Vector<3> vec23( {vec3.x() - vec2.x(), vec3.y() - vec2.y(), vec3.z() - vec2.z()} );

	Vector<4> crossProduct( {vec12.y() * vec23.z() - vec12.z() * vec23.y(),
					vec12.z() * vec23.x() - vec12.x() * vec23.z(),
					vec12.x() * vec23.y() - vec12.y() * vec23.x(),
					1.0f} );

	return crossProduct;
}

void normalizeVec4D (Vector<4>& vec)
{
	float length = sqrtf( vec.x() * vec.x() + vec.y() * vec.y() + vec.z() * vec.z() );

	vec.x() = vec.x() / length;
	vec.y() = vec.y() / length;
	vec.z() = vec.z() / length;
}

Vector<4> mulVector4DByMatrix4D (const Vector<4>& vector, const Matrix<4, 4>& matrix)
{
	Vector<4> outVec;

	outVec.x() = (vector.x() * matrix.at(0, 0)) + (vector.y() * matrix.at(1, 0)) + (vector.z() * matrix.at(2, 0)) + matrix.at(3, 0);
	outVec.y() = (vector.x() * matrix.at(0, 1)) + (vector.y() * matrix.at(1, 1)) + (vector.z() * matrix.at(2, 1)) + matrix.at(3, 1);
	outVec.z() = (vector.x() * matrix.at(0, 2)) + (vector.y() * matrix.at(1, 2)) + (vector.z() * matrix.at(2, 2)) + matrix.at(3, 2);
	outVec.w() = (vector.x() * matrix.at(0, 3)) + (vector.y() * matrix.at(1, 3)) + (vector.z() * matrix.at(2, 3)) + matrix.at(3, 3);

	if ( outVec.w() != 0.0f )
	{
		outVec.x() = outVec.x() / outVec.w();
		outVec.y() = outVec.y() / outVec.w();
		outVec.z() = outVec.z() / outVec.w();
	}

	return outVec;
}

void Mesh::scale (float scaleFactor)
{
	// TODO do all transformations with matrices soon
	for ( Face& face : faces )
	{
		Vector<4>& vec1 = face.vertices[0].vec;
		Vector<4>& vec2 = face.vertices[1].vec;
		Vector<4>& vec3 = face.vertices[2].vec;

		vec1.x() *= scaleFactor; vec1.y() *= scaleFactor; vec1.z() *= scaleFactor;
		vec2.x() *= scaleFactor; vec2.y() *= scaleFactor; vec2.z() *= scaleFactor;
		vec3.x() *= scaleFactor; vec3.y() *= scaleFactor; vec3.z() *= scaleFactor;
	}
}

void Face::calcFaceNormals()
{
	Vertex& vert1 = vertices[0];
	Vertex& vert2 = vertices[1];
	Vertex& vert3 = vertices[2];

	Vector<4> normal = crossProductVec4D( vert1.vec, vert2.vec, vert3.vec );
	normalizeVec4D( normal );

	this->normal = normal;
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

void Camera3D::projectFace (Face& face) const
{
	face.vertices[0].vec = mulVector4DByMatrix4D( face.vertices[0].vec, m_ProjectionMatrix );
	face.vertices[1].vec = mulVector4DByMatrix4D( face.vertices[1].vec, m_ProjectionMatrix );
	face.vertices[2].vec = mulVector4DByMatrix4D( face.vertices[2].vec, m_ProjectionMatrix );
}

void Camera3D::scaleXYToZeroToOne (Face& face) const
{
	face.vertices[0].vec.x() = ( face.vertices[0].vec.x() + 1.0f ) * 0.5f; face.vertices[0].vec.y() = ( face.vertices[0].vec.y() + 1.0f ) * 0.5f;
	face.vertices[1].vec.x() = ( face.vertices[1].vec.x() + 1.0f ) * 0.5f; face.vertices[1].vec.y() = ( face.vertices[1].vec.y() + 1.0f ) * 0.5f;
	face.vertices[2].vec.x() = ( face.vertices[2].vec.x() + 1.0f ) * 0.5f; face.vertices[2].vec.y() = ( face.vertices[2].vec.y() + 1.0f ) * 0.5f;
}
