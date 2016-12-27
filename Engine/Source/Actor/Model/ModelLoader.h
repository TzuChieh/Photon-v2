#pragma once

#include "Actor/Model/ModelParser/AssimpModelParser.h"

#include <assimp/importer.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ph
{

class Entity;

class ModelLoader final
{
public:
	bool load(const std::string& fullFilename, std::vector<Model>* const out_models);

private:
	AssimpModelParser m_assimpModelParser;
};

}// end namespace ph