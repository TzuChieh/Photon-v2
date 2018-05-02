#pragma once

#include "util.h"

#include <ph_core.h>

#include <string>

PH_CLI_NAMESPACE_BEGIN

class CommandLineArguments;

class StaticImageRenderer final
{
public:
	StaticImageRenderer(const CommandLineArguments& args);
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

	bool loadCommandsFromSceneFile() const;
};

PH_CLI_NAMESPACE_END