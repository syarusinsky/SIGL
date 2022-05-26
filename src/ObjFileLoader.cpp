#include "ObjFileLoader.hpp"

#include <fstream>

size_t splitStrBySpaces (const std::string& string, std::vector<std::string>& strings, char delimiter);

ObjFileLoader::ObjFileLoader()
{
}

ObjFileLoader::~ObjFileLoader()
{
}

bool ObjFileLoader::createMeshFromFile (Mesh& meshOut, const std::string& filePath)
{
	// clear previous buffers
	m_VertexBuffer.clear();
	m_TexCoordBuffer.clear();
	m_NormalBuffer.clear();
	m_FaceBuffer.clear();

	// load file or return false if fails
	std::ifstream file(filePath);
	if ( ! file ) return false;

	// load model data into buffers
	std::string tmpStr;
	while ( getline(file, tmpStr) )
	{
		std::vector<std::string> items;
		splitStrBySpaces( tmpStr, items, ' ' );

		tmpStr = items[0];
		if ( tmpStr == "v" ) // vertex
		{
			m_VertexBuffer.push_back( std::vector<float>{std::stof(items[1]), std::stof(items[2]), std::stof(items[3]), 1.0f} );
		}
		else if ( tmpStr == "vt" ) // tex coord
		{
			m_TexCoordBuffer.push_back( std::vector<float>{std::stof(items[1]), std::stof(items[2])} );
		}
		else if ( tmpStr == "vn" ) // normal
		{
			m_NormalBuffer.push_back( std::vector<float>{std::stof(items[1]), std::stof(items[2]), std::stof(items[3]), 1.0f} );
		}
		else if ( tmpStr == "f" ) // face indices
		{
			// vertex 1 indices
			std::vector<std::string> indices1;
			splitStrBySpaces( items[1], indices1, '/' );
			// vertex 2 indices
			std::vector<std::string> indices2;
			splitStrBySpaces( items[2], indices2, '/' );
			// vertex 3 indices
			std::vector<std::string> indices3;
			splitStrBySpaces( items[3], indices3, '/' );

			m_FaceBuffer.push_back(
					FaceIndices
					{
						static_cast<unsigned int>( std::stoul(indices1[0]) ),
						static_cast<unsigned int>( std::stoul(indices1[1]) ),
						static_cast<unsigned int>( std::stoul(indices1[2]) ),

						static_cast<unsigned int>( std::stoul(indices2[0]) ),
						static_cast<unsigned int>( std::stoul(indices2[1]) ),
						static_cast<unsigned int>( std::stoul(indices2[2]) ),

						static_cast<unsigned int>( std::stoul(indices3[0]) ),
						static_cast<unsigned int>( std::stoul(indices3[1]) ),
						static_cast<unsigned int>( std::stoul(indices3[2]) )
					}
			);
		}
	}

	meshOut.faces.clear();
	for ( const FaceIndices& faceIndices : m_FaceBuffer )
	{
		Vertex vert1
		{
			m_VertexBuffer[faceIndices.vertexIndex1 - 1],
			m_NormalBuffer[faceIndices.normalIndex1 - 1],
			m_TexCoordBuffer[faceIndices.texCoordIndex1 - 1]
		};
		Vertex vert2
		{
			m_VertexBuffer[faceIndices.vertexIndex2 - 1],
			m_NormalBuffer[faceIndices.normalIndex2 - 1],
			m_TexCoordBuffer[faceIndices.texCoordIndex2 - 1]
		};
		Vertex vert3
		{
			m_VertexBuffer[faceIndices.vertexIndex3 - 1],
			m_NormalBuffer[faceIndices.normalIndex3 - 1],
			m_TexCoordBuffer[faceIndices.texCoordIndex3 - 1]
		};

		meshOut.faces.push_back( Face{{vert1, vert2, vert3}} );
	}

	for ( Face& face : meshOut.faces )
	{
		face.calcFaceNormals();
	}

	return true;
}

size_t splitStrBySpaces (const std::string& string, std::vector<std::string>& strings, char delimiter)
{
	size_t pos = string.find( delimiter );
	size_t initialPos = 0;
	strings.clear();

	while ( pos != std::string::npos )
	{
		strings.push_back( string.substr(initialPos, pos - initialPos) );
		initialPos = pos + 1;

		pos = string.find( delimiter, initialPos );
	}

	strings.push_back( string.substr(initialPos, std::min(pos, string.size()) - initialPos + 1) );

	return strings.size();
}
