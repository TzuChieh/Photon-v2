#include "FileIO/TextFileLoader.h"
#include "Common/assertion.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace ph
{

const Logger TextFileLoader::logger(LogSender("Text File Loader"));

bool TextFileLoader::load(const Path& filePath, std::string* const out_text)
{
	PH_ASSERT(out_text);

	logger.log(ELogLevel::NOTE_MED, 
	           "loading text file <" + filePath.toString() + ">");

	std::ifstream textFile;
	textFile.open(filePath.toAbsoluteString());
	if(!textFile.is_open())
	{
		logger.log(ELogLevel::WARNING_MED, 
		           "cannot open text file <"+ filePath.toAbsoluteString() + ">");
		return false;
	}

	std::stringstream buffer;
	buffer << textFile.rdbuf();
	*out_text = buffer.str();

	return true;
}

}// end namespace ph