#include "Model/ModelParser/AssimpModelParser.h"
#include "Math/Vector3f.h"
#include "Model/Geometry/GTriangle.h"
#include "Model/Model.h"
#include "Model/Material/MatteOpaque.h"
#include "Model/Geometry/GTriangleMesh.h"
#include "Model/Material/PerfectMirror.h"
#include "Model/TextureMapper/DefaultMapper.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <iostream>

namespace ph
{

AssimpModelParser::~AssimpModelParser() = default;

bool AssimpModelParser::parse(const std::string& fullFilename, Model* const out_model)
{
	const aiScene* assimpScene = m_assimpImporter.ReadFile(fullFilename, aiProcess_Triangulate);
	if(!assimpScene)
	{
		std::cerr << "warning: at ModelLoader::load(), model <" << fullFilename << " loading failed" << std::endl;
		std::cerr << "(message: " << m_assimpImporter.GetErrorString() << ")" << std::endl;
		return false;
	}

	std::vector<Vector3f> positions;
	std::vector<Vector3f> normals;
	std::vector<uint32>   indices;

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
		return false;
	}

	auto geometry = std::make_shared<GTriangleMesh>();
	auto material = std::make_shared<MatteOpaque>();
	material->setAlbedo(1, 1, 1);
	//auto material = std::make_shared<PerfectMirror>();

	for(std::size_t i = 0; i < indices.size(); i += 3)
	{
		geometry->addTriangle(GTriangle(positions[indices[i]], positions[indices[i + 1]], positions[indices[i + 2]]));
	}

	out_model->setGeometry(geometry);
	out_model->setMaterial(material);
	out_model->setTextureMapper(std::make_shared<DefaultMapper>());

	return true;
}

}// end namespace ph