#include "DataIO/IOUtils.h"
#include "DataIO/io_exceptions.h"
#include "Common/assertion.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ph
{

void IOUtils::loadText(const Path& filePath, std::string* const out_text)
{
	PH_ASSERT(out_text);

	std::ifstream textFile;
	textFile.open(filePath.toAbsoluteString());
	if(!textFile.is_open())
	{
		throw FileIOError("cannot open text file <" + filePath.toAbsoluteString() + ">");
	}

	// OPTIMIZATION: a redundant copy here
	std::stringstream buffer;
	buffer << textFile.rdbuf();
	*out_text = buffer.str();
}

}// end namespace ph
