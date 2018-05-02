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

	void render() const;

private:
	PHuint64    m_engineId;
	std::string m_sceneFilePath;
	std::string m_imageFilePath;
	int         m_numRenderThreads;
	bool        m_performPostProcessing;

	bool loadCommandsFromSceneFile() const;
};

PH_CLI_NAMESPACE_END