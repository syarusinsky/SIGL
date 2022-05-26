#ifndef OBJFILELOADER_HPP
#define OBJFILELOADER_HPP

#include "Engine3D.hpp"

#include <vector>
#include <string>

struct FaceIndices
{
	unsigned int vertexIndex1;
	unsigned int texCoordIndex1;
	unsigned int normalIndex1;

	unsigned int vertexIndex2;
	unsigned int texCoordIndex2;
	unsigned int normalIndex2;

	unsigned int vertexIndex3;
	unsigned int texCoordIndex3;
	unsigned int normalIndex3;
};

class ObjFileLoader
{
	public:
		ObjFileLoader();
		~ObjFileLoader();

		bool createMeshFromFile (Mesh& meshOut, const std::string& filePath);

	private:
		std::vector<Vector<4>> 		m_VertexBuffer;
		std::vector<Vector<2>> 		m_TexCoordBuffer;
		std::vector<Vector<4>> 		m_NormalBuffer;
		std::vector<FaceIndices> 	m_FaceBuffer;
};

#endif // OBJFILELOADER_HPP
