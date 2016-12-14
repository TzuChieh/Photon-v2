#pragma once

#include "Entity/ModelParser/AssimpModelParser.h"

#include <assimp/importer.hpp>

#include <memory>
#include <string>

namespace ph
{

class Entity;

class ModelLoader final
{
public:
	bool load(const std::string& fullFilename, Entity* const out_entity);

private:
	AssimpModelParser m_assimpModelParser;
};

}// end namespace ph