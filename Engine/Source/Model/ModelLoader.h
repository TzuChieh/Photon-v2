#pragma once

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
	Assimp::Importer m_assimpImporter;
};

}// end namespace ph