#pragma once

#include "DataIO/FileSystem/Path.h"

#include <utiltiy>

namespace ph
{

class SdlInputContext final
{
public:
	explicit SdlInputContext(Path workingDirectory);

	Path workingDirectory;
};

// In-header Implementation:

inline SdlInputContext::SdlInputContext(Path workingDirectory) : 
	workingDirectory(workingDirectory)
{}

}// end namespace ph
