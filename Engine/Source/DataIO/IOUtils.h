#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class IOUtils final
{
public:
	static std::string loadText(const Path& filePath);

	// TODO: loadBytes()
};

}// end namespace ph
