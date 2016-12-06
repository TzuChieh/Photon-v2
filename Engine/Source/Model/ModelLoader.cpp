#include "Model/ModelLoader.h"

#include <iostream>

namespace ph
{

bool ModelLoader::load(const std::string& fullFilename, Model* const out_model)
{
	std::cout << "loading model <" << fullFilename << ">" << std::endl;

	return m_assimpModelParser.parse(fullFilename, out_model);
}

}// end namespace ph