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

	void render() const;

	void setSceneFilePath(const std::string& path);
	void setImageFilePath(const std::string& path);

private:
	PHuint64    m_engineId;
	std::string m_sceneFilePath;
	std::string m_imageFilePath;
	int         m_numRenderThreads;
	bool        m_isPostProcessRequested;
	float       m_outputPercentageProgress;

	bool loadCommandsFromSceneFile() const;
};

PH_CLI_NAMESPACE_END