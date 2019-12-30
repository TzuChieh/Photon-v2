#pragma once

#include "util.h"
#include "ProcessedArguments.h"

#include <ph_core.h>

#include <string>

PH_CLI_NAMESPACE_BEGIN

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

PH_CLI_NAMESPACE_END
