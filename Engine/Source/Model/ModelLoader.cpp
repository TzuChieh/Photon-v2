#include "Model/ModelLoader.h"
#include "Math/Vector3f.h"
#include "Model/Geometry/GTriangle.h"
#include "Model/Model.h"
#include "Model/Material/MatteOpaque.h"
#include "Model/Geometry/GTriangleMesh.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <iostream>

namespace ph
{

std::unique_ptr<Model> ModelLoader::load(const std::string& fullFilename)
{
	const aiScene* assimpScene = m_assimpImporter.ReadFile(fullFilename, aiProcess_Triangulate);
	if(!assimpScene)
	{
		std::cerr << m_assimpImporter.GetErrorString() << std::endl;
		return nullptr;
	}

	std::vector<Vector3f> positions;
	std::vector<Vector3f> normals;
	std::vector<uint32>  indices;

	// FIXME: mMeshes[N]
	const aiMesh* mesh = assimpScene->mMeshes[0];

	if(mesh->HasPositions())
	{
		for(int i = 0; i < mesh->mNumVertices; ++i)
		{
			positions.push_back(Vector3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		}
	}

	if(mesh->HasNormals())
	{
		for(int i = 0; i < mesh->mNumVertices; ++i)
		{
			normals.push_back(Vector3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		}
	}

	if(mesh->HasFaces())
	{
		for(int i = 0; i < mesh->mNumFaces; ++i)
		{
			indices.push_back(mesh->mFaces[i].mIndices[0]);
			indices.push_back(mesh->mFaces[i].mIndices[1]);
			indices.push_back(mesh->mFaces[i].mIndices[2]);
		}
	}

	if(positions.empty() || normals.empty() || indices.empty() || indices.size() % 3 != 0)
	{
		std::cerr << "ModelLoader error: unsupported format" << std::endl;
		return nullptr;
	}

	auto geometry = std::make_shared<GTriangleMesh>();
	auto material = std::make_shared<MatteOpaque>();

	for(std::size_t i = 0; i < indices.size(); i += 3)
	{
		geometry->addTriangle(std::make_shared<GTriangle>(positions[indices[i]], positions[indices[i + 1]], positions[indices[i + 2]]));
	}

	return std::make_unique<Model>(geometry, material);
}

}// end namespace ph