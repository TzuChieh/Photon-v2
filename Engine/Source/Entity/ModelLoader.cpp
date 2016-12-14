#include "Entity/ModelLoader.h"

#include <iostream>

namespace ph
{

bool ModelLoader::load(const std::string& fullFilename, Entity* const out_entity)
{
	std::cout << "loading model <" << fullFilename << ">" << std::endl;

	return m_assimpModelParser.parse(fullFilename, out_entity);
}

}// end namespace ph