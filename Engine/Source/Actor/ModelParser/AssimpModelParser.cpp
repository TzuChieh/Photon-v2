#include "Actor/ModelParser/AssimpModelParser.h"
#include "Math/Vector3f.h"
#include "Actor/Geometry/GTriangle.h"
#include "Actor/AModel.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/TextureMapper/DefaultMapper.h"
#include "Actor/ModelParser/AiMeshParser.h"
#include "Actor/ModelParser/AiMaterialParser.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <iostream>
#include <memory>

namespace ph
{

AssimpModelParser::~AssimpModelParser() = default;

bool AssimpModelParser::parse(const std::string& fullFilename, std::vector<AModel>* const out_models)
{
	const aiScene* assimpScene = m_assimpImporter.ReadFile(fullFilename, aiProcess_Triangulate);
	if(!assimpScene)
	{
		std::cerr << "warning: at ModelLoader::load(), model <" << fullFilename << " loading failed" << std::endl;
		std::cerr << "(message: " << m_assimpImporter.GetErrorString() << ")" << std::endl;
		return false;
	}

	const std::string& modelDirectory = fullFilename.substr(0, fullFilename.find_last_of('/') + 1);
	AModel rootModel;

	std::vector<std::shared_ptr<Geometry>> geometries;
	std::vector<std::shared_ptr<Material>> materials;

	if(assimpScene->HasMeshes())
	{
		for(uint32 i = 0; i < assimpScene->mNumMeshes; i++)
		{
			geometries.push_back(AiMeshParser::parse(assimpScene->mMeshes[i]));
		}
	}

	if(assimpScene->HasMaterials())
	{
		for(uint32 i = 0; i < assimpScene->mNumMaterials; i++)
		{
			materials.push_back(AiMaterialParser::parseMaterial(assimpScene->mMaterials[i], modelDirectory));
		}
	}

	for(uint32 i = 0; i < assimpScene->mNumMeshes; i++)
	{
		out_models->push_back(AModel(geometries[i], materials[assimpScene->mMeshes[i]->mMaterialIndex]));
	}

	return true;
}

}// end namespace ph