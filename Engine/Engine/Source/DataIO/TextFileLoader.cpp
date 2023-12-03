#include "DataIO/TextFileLoader.h"
#include "Common/assertion.h"
#include "Common/logging.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(TextFileLoader, DataIO);

bool TextFileLoader::load(const Path& filePath, std::string* const out_text)
{
	PH_ASSERT(out_text);

	PH_LOG(TextFileLoader, "loading text file <{}>", filePath.toString());

	std::ifstream textFile;
	textFile.open(filePath.toNativeString());
	if(!textFile.is_open())
	{
		PH_LOG_WARNING(TextFileLoader, "cannot open text file <{}>", filePath.toAbsoluteString());

		return false;
	}

	std::stringstream buffer;
	buffer << textFile.rdbuf();
	*out_text = buffer.str();

	return true;
}

}// end namespace ph
