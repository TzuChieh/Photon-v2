#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"
#include "Common/assertion.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ph::io_utils
{

std::string load_text(const Path& filePath)
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

}// end namespace ph::io_utils
