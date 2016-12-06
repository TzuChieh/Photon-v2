#pragma once

#include "Model/ModelParser/ModelParser.h"

#include <assimp/importer.hpp>

#include <memory>
#include <string>

namespace ph
{

class AssimpModelParser final : ModelParser
{
public:
	virtual ~AssimpModelParser() override;

	virtual bool parse(const std::string& fullFilename, Model* const out_model) override;

private:
	Assimp::Importer m_assimpImporter;
};

}// end namespace ph