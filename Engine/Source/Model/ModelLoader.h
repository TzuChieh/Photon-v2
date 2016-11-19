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
	std::unique_ptr<Model> load(const std::string& fullFilename);

private:
	Assimp::Importer m_assimpImporter;
};

}// end namespace ph