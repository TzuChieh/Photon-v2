#include "FileIO/RenderDataFileLoader.h"

#include <iostream>

#define KEYWORD_ENTITY_BEGIN "EntityBegin"
#define KEYWORD_ENTITY_END   "EntityEnd"

namespace ph
{

RenderDataFileLoader::RenderDataFileLoader()
{

}

bool RenderDataFileLoader::load(const std::string& fullFilename)
{
	std::ifstream dataFile;
	dataFile.open(fullFilename, std::ios::in);
	if(!dataFile.is_open())
	{
		std::cerr << "warning: at RenderDataFileLoader::load(), failed to open file <" + fullFilename + ">" << std::endl;
		return false;
	}
	else
	{
		clearLoadedData();
		parseDataFile(&dataFile);
		dataFile.close();
		return true;
	}
}

void RenderDataFileLoader::parseDataFile(std::ifstream* const dataFile)
{
	std::string lineString;
	std::string sourceString;
	while(dataFile->good())
	{
		std::getline(*dataFile, lineString);
		sourceString.append(lineString + '\n');
	}

	std::cout << sourceString << std::endl;
}

std::vector<Entity> RenderDataFileLoader::getLoadedEntities() const
{
	return m_loadedEntities;
}

void RenderDataFileLoader::clearLoadedData()
{
	m_loadedEntities.clear();
	m_loadedEntities.shrink_to_fit();
}

}// end namespace ph