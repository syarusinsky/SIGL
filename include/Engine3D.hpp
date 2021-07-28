#ifndef ENGINE3D_HPP
#define ENGING3D_HPP

/**************************************************************************
 * This Engine3D file defines the classes, structures, and functions
 * necessary to create 3D graphics with SIGL.
**************************************************************************/

#include <vector>

struct Vector3D
{
    float x;
    float y;
    float z;
};

struct Matrix4D
{
    float matrix[4][4] = { 0.0f };
};

Vector3D mulVector3DByMatrix4D (const Vector3D& vector, const Matrix4D& matrix);

struct Vertex
{
    Vector3D vec;
};

struct Face
{
    Vertex vertices[3];
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

    private:
        float       m_NearClip;
        float       m_FarClip;
        float       m_FieldOfView;
        float       m_AspectRatio;

        Matrix4D    m_ProjectionMatrix;

        void generateProjectionMatrix();
};

Mesh createCubeMesh();

#endif // ENGINE3D_HPP