#pragma once

#include "Actor/ModelParser/AssimpModelParser.h"

#include <assimp/importer.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ph
{

class ModelLoader final
{
public:
	bool load(const std::string& fullFilename, std::vector<AModel>* const out_models);

private:
	AssimpModelParser m_assimpModelParser;
};

}// end namespace ph