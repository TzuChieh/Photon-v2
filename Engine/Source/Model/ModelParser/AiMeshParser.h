#pragma once

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

namespace ph
{

class Model;

class AiMeshParser
{
public:
	static bool parse(const aiMesh* const mesh, Model* const out_model);
};

}// end namespace ph