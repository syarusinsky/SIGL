#ifndef ENGINE3D_HPP
#define ENGINE3D_HPP

/**************************************************************************
 * This Engine3D file defines the classes, structures, and functions
 * necessary to create 3D graphics with SIGL.
**************************************************************************/

#include <vector>
#include <Matrix.hpp>
#include <Vector.hpp>

Vector<4> mulVector4DByMatrix4D (const Vector<4>& vector, const Matrix<4, 4>& matrix);

Matrix<4, 4> generateIdentityMatrix();
Matrix<4, 4> generateRotationMatrix (float xDegrees, float yDegrees, float zDegrees);

struct Vertex
{
	Vector<4> vec;
	Vector<4> normal;
	Vector<2> texCoords;
};

struct Face
{
	Vertex vertices[3];

	Vector<4> calcFaceNormals();
};

struct Mesh
{
	std::vector<Face> faces;
	Matrix<4, 4> transformMat = generateIdentityMatrix();

	// operations on tranformation matrix
	void scale (float scaleFactor);
	void translate (float x, float y, float z);
	void rotate (float x, float y, float z);
};

struct PointLight
{
	Vector<4> position;
	Vector<3> color;
	float intensity;
};

class Camera3D
{
	public:
		Camera3D (float nearClip, float farClip, float fieldOfView, float aspectRatio);

		void projectFace (Face& face) const;

		// scales the x and y vertices to 0.0f -> 1.0f instead of projected -1.0f to 1.0f
		void scaleXYToZeroToOne (Face& face) const;

		float x() const;
		float y() const;
		float z() const;

		float getNearClip() { return m_NearClip; }
		float getFarClip() { return m_FarClip; }

	private:
		float			m_NearClip;
		float			m_FarClip;
		float			m_FieldOfView;
		float			m_AspectRatio;

		Matrix<4, 4>	m_ProjectionMatrix;

		Vector<4> 		m_Position;

		void generateProjectionMatrix();
};

Mesh createCubeMesh();

#endif // ENGINE3D_HPP
