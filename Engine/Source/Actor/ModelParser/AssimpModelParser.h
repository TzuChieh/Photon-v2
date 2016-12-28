#pragma once

#include "Actor/ModelParser/ModelParser.h"

#include <assimp/importer.hpp>

#include <memory>
#include <string>

namespace ph
{

class AssimpModelParser final : public ModelParser
{
public:
	virtual ~AssimpModelParser() override;

	virtual bool parse(const std::string& fullFilename, std::vector<AModel>* const out_models) override;

private:
	Assimp::Importer m_assimpImporter;
};

}// end namespace ph