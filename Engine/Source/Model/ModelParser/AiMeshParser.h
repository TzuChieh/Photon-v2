#pragma once

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <memory>

namespace ph
{

class Geometry;

class AiMeshParser final
{
public:
	static std::shared_ptr<Geometry> parse(const aiMesh* const mesh);
};

}// end namespace ph