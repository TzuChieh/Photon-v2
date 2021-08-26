#pragma once

#include "DataIO/FileSystem/Path.h"

namespace ph { class SceneDescription; }

namespace ph
{

class SdlWriter final
{
public:
	SdlWriter();
	explicit SdlWriter(Path workingDirectory);

	void write(const SceneDescription& scene);

private:
	Path m_workingDirectory;
};

}// end namespace ph
