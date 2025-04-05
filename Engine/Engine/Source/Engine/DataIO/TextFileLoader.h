#pragma once

#include "Engine/DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class TextFileLoader final
{
public:
	static bool load(const Path& filePath, std::string* out_text);
};

}// end namespace ph
