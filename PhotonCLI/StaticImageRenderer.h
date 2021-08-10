#pragma once

#include "util.h"
#include "ProcessedArguments.h"

#include <ph_core.h>

#include <string>

namespace ph::cli
{

class StaticImageRenderer final
{
public:
	explicit StaticImageRenderer(const ProcessedArguments& args);
	~StaticImageRenderer();

	void render();

	void setSceneFilePath(const std::string& path);
	void setImageOutputPath(const std::string& path);

private:
	PHuint64           m_engineId;
	ProcessedArguments m_args;

	bool loadCommandsFromSceneFile() const;
};

}// end namespace ph::cli
