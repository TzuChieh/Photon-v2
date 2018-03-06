#pragma once

#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"

#include <string>

namespace ph
{

class TextFileLoader final
{
public:
	static bool load(const Path& filePath, std::string* out_text);

private:
	static const Logger logger;
};

}// end namespace ph