#pragma once

#include "Model/ModelParser/AssimpModelParser.h"

#include <assimp/importer.hpp>

#include <memory>
#include <string>

namespace ph
{

class Model;

class ModelLoader final
{
public:
	bool load(const std::string& fullFilename, Model* const out_model);

private:
	AssimpModelParser m_assimpModelParser;
};

}// end namespace ph