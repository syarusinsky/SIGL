#include "Engine3D.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

Mesh createCubeMesh()
{
    Mesh cube;
    cube.faces =
    {
        // front face
        Face{ Vertex{Vector3D{0.0f, 0.0f, 0.0f}}, Vertex{Vector3D{0.0f, 1.0f, 0.0f}}, Vertex{Vector3D{1.0f, 1.0f, 0.0f}} },
        Face{ Vertex{Vector3D{0.0f, 0.0f, 0.0f}}, Vertex{Vector3D{1.0f, 1.0f, 0.0f}}, Vertex{Vector3D{1.0f, 0.0f, 0.0f}} },

        // right face
        Face{ Vertex{Vector3D{1.0f, 0.0f, 0.0f}}, Vertex{Vector3D{1.0f, 1.0f, 0.0f}}, Vertex{Vector3D{1.0f, 1.0f, 1.0f}} },
        Face{ Vertex{Vector3D{1.0f, 0.0f, 0.0f}}, Vertex{Vector3D{1.0f, 1.0f, 1.0f}}, Vertex{Vector3D{1.0f, 0.0f, 1.0f}} },

        // back face
        Face{ Vertex{Vector3D{1.0f, 0.0f, 1.0f}}, Vertex{Vector3D{1.0f, 1.0f, 1.0f}}, Vertex{Vector3D{0.0f, 1.0f, 1.0f}} },
        Face{ Vertex{Vector3D{1.0f, 0.0f, 1.0f}}, Vertex{Vector3D{0.0f, 1.0f, 1.0f}}, Vertex{Vector3D{0.0f, 0.0f, 1.0f}} },

        // left face
        Face{ Vertex{Vector3D{0.0f, 0.0f, 1.0f}}, Vertex{Vector3D{0.0f, 1.0f, 1.0f}}, Vertex{Vector3D{0.0f, 1.0f, 0.0f}} },
        Face{ Vertex{Vector3D{0.0f, 0.0f, 1.0f}}, Vertex{Vector3D{0.0f, 1.0f, 0.0f}}, Vertex{Vector3D{0.0f, 0.0f, 0.0f}} },

        // top face
        Face{ Vertex{Vector3D{0.0f, 1.0f, 0.0f}}, Vertex{Vector3D{0.0f, 1.0f, 1.0f}}, Vertex{Vector3D{1.0f, 1.0f, 1.0f}} },
        Face{ Vertex{Vector3D{0.0f, 1.0f, 0.0f}}, Vertex{Vector3D{1.0f, 1.0f, 1.0f}}, Vertex{Vector3D{1.0f, 1.0f, 0.0f}} },

        // bottom face
        Face{ Vertex{Vector3D{0.0f, 0.0f, 1.0f}}, Vertex{Vector3D{0.0f, 0.0f, 0.0f}}, Vertex{Vector3D{1.0f, 0.0f, 0.0f}} },
        Face{ Vertex{Vector3D{0.0f, 0.0f, 1.0f}}, Vertex{Vector3D{1.0f, 0.0f, 0.0f}}, Vertex{Vector3D{1.0f, 0.0f, 1.0f}} },
    };

    return cube;
}

Vector3D mulVector3DByMatrix4D (const Vector3D& vector, const Matrix4D& matrix)
{
    Vector3D outVec;

    outVec.x = (vector.x * matrix.matrix[0][0]) + (vector.y * matrix.matrix[1][0]) + (vector.z * matrix.matrix[2][0]) + matrix.matrix[3][0];
    outVec.y = (vector.x * matrix.matrix[0][1]) + (vector.y * matrix.matrix[1][1]) + (vector.z * matrix.matrix[2][1]) + matrix.matrix[3][1];
    outVec.z = (vector.x * matrix.matrix[0][2]) + (vector.y * matrix.matrix[1][2]) + (vector.z * matrix.matrix[2][2]) + matrix.matrix[3][2];
    float w = (vector.x * matrix.matrix[0][3]) + (vector.y * matrix.matrix[1][3]) + (vector.z * matrix.matrix[2][3]) + matrix.matrix[3][3];

    if ( w != 0.0f )
    {
        outVec.x = outVec.x / w;
        outVec.y = outVec.y / w;
        outVec.z = outVec.z / w;
    }

    return outVec;
}

void Mesh::scale (float scaleFactor)
{
    for ( Face& face : faces )
    {
        Vector3D& vec1 = face.vertices[0].vec;
        Vector3D& vec2 = face.vertices[1].vec;
        Vector3D& vec3 = face.vertices[2].vec;

        vec1.x *= scaleFactor; vec1.y *= scaleFactor; vec1.z *= scaleFactor;
        vec2.x *= scaleFactor; vec2.y *= scaleFactor; vec2.z *= scaleFactor;
        vec3.x *= scaleFactor; vec3.y *= scaleFactor; vec3.z *= scaleFactor;
    }
}

Camera3D::Camera3D (float nearClip, float farClip, float fieldOfView, float aspectRatio) :
    m_NearClip( nearClip ),
    m_FarClip( farClip ),
    m_FieldOfView( fieldOfView ),
    m_AspectRatio( aspectRatio ),
    m_ProjectionMatrix()
{
    this->generateProjectionMatrix();
}

void Camera3D::generateProjectionMatrix()
{
    float projectionPlaneDistance = 1.0f / tanf( m_FieldOfView * 0.5f / 180.0f * M_PI ); // also converting fov to radians
    float normalizedClip = m_FarClip / ( m_FarClip - m_NearClip );

    m_ProjectionMatrix = Matrix4D(); // reset the projection matrix
    m_ProjectionMatrix.matrix[0][0] = projectionPlaneDistance * m_AspectRatio;
    m_ProjectionMatrix.matrix[1][1] = projectionPlaneDistance;
    m_ProjectionMatrix.matrix[2][2] = normalizedClip;
    m_ProjectionMatrix.matrix[3][2] = (m_NearClip * -1.0f) * normalizedClip;
    m_ProjectionMatrix.matrix[2][3] = 1.0f;
    m_ProjectionMatrix.matrix[3][3] = 0.0f;
}

Face Camera3D::projectFace (const Face& face)
{
    Face outFace;
    outFace.vertices[0] = face.vertices[0];
    outFace.vertices[1] = face.vertices[1];
    outFace.vertices[2] = face.vertices[2];

    outFace.vertices[0].vec = mulVector3DByMatrix4D( outFace.vertices[0].vec, m_ProjectionMatrix );
    outFace.vertices[1].vec = mulVector3DByMatrix4D( outFace.vertices[1].vec, m_ProjectionMatrix );
    outFace.vertices[2].vec = mulVector3DByMatrix4D( outFace.vertices[2].vec, m_ProjectionMatrix );

    return outFace;
}

void Camera3D::scaleXYToZeroToOne (Face& face)
{
    face.vertices[0].vec.x = ( face.vertices[0].vec.x + 1.0f ) * 0.5f; face.vertices[0].vec.y = ( face.vertices[0].vec.y + 1.0f ) * 0.5f;
    face.vertices[1].vec.x = ( face.vertices[1].vec.x + 1.0f ) * 0.5f; face.vertices[1].vec.y = ( face.vertices[1].vec.y + 1.0f ) * 0.5f;
    face.vertices[2].vec.x = ( face.vertices[2].vec.x + 1.0f ) * 0.5f; face.vertices[2].vec.y = ( face.vertices[2].vec.y + 1.0f ) * 0.5f;
}