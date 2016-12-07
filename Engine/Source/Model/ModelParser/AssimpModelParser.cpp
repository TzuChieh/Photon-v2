#include "Model/ModelParser/AssimpModelParser.h"
#include "Math/Vector3f.h"
#include "Model/Geometry/GTriangle.h"
#include "Model/Model.h"
#include "Model/Material/MatteOpaque.h"
#include "Model/Geometry/GTriangleMesh.h"
#include "Model/Material/PerfectMirror.h"
#include "Model/TextureMapper/DefaultMapper.h"
#include "Model/ModelParser/AiMeshParser.h"
#include "Model/ModelParser/AiMaterialParser.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <iostream>
#include <memory>

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

	const std::string& modelDirectory = fullFilename.substr(0, fullFilename.find_last_of('/') + 1);
	Model rootModel;

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
		rootModel.addChild(Model(geometries[i], materials[assimpScene->mMeshes[i]->mMaterialIndex]));
	}

	*out_model = rootModel;

	return true;
}

}// end namespace ph