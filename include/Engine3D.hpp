#ifndef ENGINE3D_HPP
#define ENGING3D_HPP

/**************************************************************************
 * This Engine3D file defines the classes, structures, and functions
 * necessary to create 3D graphics with SIGL.
**************************************************************************/

#include <vector>
#include <Matrix.hpp>
#include <Vector.hpp>

Vector<3> mulVector3DByMatrix4D (const Vector<3>& vector, const Matrix<4, 4>& matrix);

Matrix<4, 4> generateRotationMatrix (float xDegrees, float yDegrees, float zDegrees);

struct Vertex
{
	Vector<3> vec;
};

struct Face
{
	Vertex vertices[3];
	Vector<3> normal;

	void calcNormals();
};

struct Mesh
{
	std::vector<Face> faces;

	void scale (float scaleFactor);
};

class Camera3D
{
	public:
		Camera3D (float nearClip, float farClip, float fieldOfView, float aspectRatio);

		Face projectFace (const Face& face);

		// scales the x and y vertices to 0.0f -> 1.0f instead of projected -1.0f to 1.0f
		void scaleXYToZeroToOne (Face& face);

		float x() const;
		float y() const;
		float z() const;

	private:
		float			m_NearClip;
		float			m_FarClip;
		float			m_FieldOfView;
		float			m_AspectRatio;

		Matrix<4, 4>	m_ProjectionMatrix;

		Vector<3> 		m_Position;

		void generateProjectionMatrix();
};

Mesh createCubeMesh();

#endif // ENGINE3D_HPP