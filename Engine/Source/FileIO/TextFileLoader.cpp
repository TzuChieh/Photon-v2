#include "FileIO/TextFileLoader.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ph
{

bool TextFileLoader::load(const Path& filePath, std::string* const out_text)
{
	std::cout << "loading text file <" << filePath.toString() << ">" << std::endl;

	std::ifstream textFile;
	textFile.open(filePath.toAbsoluteString());
	if(!textFile.is_open())
	{
		std::cerr << "warning: cannot open text file <" 
		          << filePath.toAbsoluteString() << ">" << std::endl;
		return false;
	}

	std::stringstream buffer;
	buffer << textFile.rdbuf();
	*out_text = buffer.str();

	return true;
}

}// end namespace ph