#include "FileIO/RenderingFileLoader.h"
#include "FileIO/RenderingDescription.h"

#include <iostream>

namespace ph
{

RenderingFileLoader::RenderingFileLoader()
{

}

bool RenderingFileLoader::load(const std::string& fullFilename, RenderingDescription* const out_data)
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
		parseCoreCommands(dataFile);
		parseWorldCommands(dataFile);
		dataFile.close();
		return true;
	}
}

void RenderingFileLoader::parseCoreCommands(std::ifstream& dataFile)
{

}

void RenderingFileLoader::parseWorldCommands(std::ifstream& dataFile)
{
	std::string lineString;
	std::string sourceString;
	while(dataFile.good())
	{
		std::getline(dataFile, lineString);
		sourceString.append(lineString + '\n');
	}

	std::cout << sourceString << std::endl;
}

}// end namespace ph