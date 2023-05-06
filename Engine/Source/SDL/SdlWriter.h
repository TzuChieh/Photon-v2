#pragma once

#include "DataIO/FileSystem/Path.h"
#include "SDL/SdlReferenceResolver.h"

#include <string>

namespace ph { class SceneDescription; }

namespace ph
{

class SdlWriter final
{
public:
	SdlWriter();
	SdlWriter(std::string sceneName, Path workingDirectory);

	void write(const SceneDescription& scene);

private:
	std::string          m_sceneName;
	Path                 m_workingDirectory;
	SdlReferenceResolver m_refResolver;
};

}// end namespace ph
