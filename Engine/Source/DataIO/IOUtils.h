#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class IOUtils final
{
public:
	static void loadText(const Path& filePath, std::string* out_text);

	// TODO: loadBytes()
};

}// end namespace ph
