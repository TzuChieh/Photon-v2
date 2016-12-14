#include "Entity/ModelParser/AssimpModelParser.h"
#include "Math/Vector3f.h"
#include "Entity/Geometry/GTriangle.h"
#include "Entity/Entity.h"
#include "Entity/Material/MatteOpaque.h"
#include "Entity/Geometry/GTriangleMesh.h"
#include "Entity/Material/PerfectMirror.h"
#include "Entity/TextureMapper/DefaultMapper.h"
#include "Entity/ModelParser/AiMeshParser.h"
#include "Entity/ModelParser/AiMaterialParser.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <iostream>
#include <memory>

namespace ph
{

AssimpModelParser::~AssimpModelParser() = default;

bool AssimpModelParser::parse(const std::string& fullFilename, Entity* const out_entity)
{
	const aiScene* assimpScene = m_assimpImporter.ReadFile(fullFilename, aiProcess_Triangulate);
	if(!assimpScene)
	{
		std::cerr << "warning: at ModelLoader::load(), model <" << fullFilename << " loading failed" << std::endl;
		std::cerr << "(message: " << m_assimpImporter.GetErrorString() << ")" << std::endl;
		return false;
	}

	const std::string& modelDirectory = fullFilename.substr(0, fullFilename.find_last_of('/') + 1);
	Entity rootModel;

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
		rootModel.addChild(Entity(geometries[i], materials[assimpScene->mMeshes[i]->mMaterialIndex]));
	}

	*out_entity = rootModel;

	return true;
}

}// end namespace ph