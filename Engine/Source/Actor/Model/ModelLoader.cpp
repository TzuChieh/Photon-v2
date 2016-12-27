#include "Actor/Model/ModelLoader.h"

#include <iostream>

namespace ph
{

bool ModelLoader::load(const std::string& fullFilename, std::vector<Model>* const out_models)
{
	std::cout << "loading model <" << fullFilename << ">" << std::endl;

	return m_assimpModelParser.parse(fullFilename, out_models);
}

}// end namespace ph