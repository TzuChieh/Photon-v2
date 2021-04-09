#include "DataIO/IOUtils.h"
#include "DataIO/io_exceptions.h"
#include "Common/assertion.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ph
{

std::string IOUtils::loadText(const Path& filePath)
{
	std::ifstream textFile;
	textFile.open(filePath.toAbsoluteString());
	if(!textFile.is_open())
	{
		throw FileIOError("cannot open text file <" + filePath.toAbsoluteString() + ">");
	}

	// OPTIMIZATION: a redundant copy here
	std::stringstream buffer;
	buffer << textFile.rdbuf();
	return buffer.str();
}

}// end namespace ph
