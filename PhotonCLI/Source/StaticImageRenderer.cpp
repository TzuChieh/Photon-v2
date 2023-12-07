#include "StaticImageRenderer.h"

#include <asio.hpp>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <limits>
#include <cstdint>

// FIXME: add osx fs headers once it is supported
#if defined(_WIN32)
	#include <filesystem>
#elif defined(__linux__)
	#include <experimental/filesystem>
#endif

namespace ph::cli
{

StaticImageRenderer::StaticImageRenderer(const ProcessedArguments& args)
	: m_engineId(0)
	, m_args(args)
{
	phCreateEngine(&m_engineId, static_cast<PhUInt32>(args.numThreads()));
}

StaticImageRenderer::~StaticImageRenderer()
{
	phDeleteEngine(m_engineId);
}

void StaticImageRenderer::setSceneFilePath(const std::string& path)
{
	m_args.setSceneFilePath(path);

// REFACTOR: use a getReference directory-from-file-path function
#ifndef __APPLE__
	namespace fs = std::filesystem;
	const std::string sceneDirectory = fs::path(path).parent_path().string();
	phSetWorkingDirectory(m_engineId, sceneDirectory.c_str());
#else
	const std::size_t slashIndex = path.find_last_of('/');
	if(slashIndex != std::string::npos)
	{
		const std::string sceneDirectory = path.substr(0, slashIndex + 1);
		phSetWorkingDirectory(m_engineId, sceneDirectory.c_str());
	}
	else
	{
		std::cerr << "warning: cannot retrieve scene directory from path <" << path << ">" << std::endl;
	}
#endif
}

void StaticImageRenderer::setImageOutputPath(const std::string& path)
{
	m_args.setImageOutputPath(path);
}

bool StaticImageRenderer::loadCommandsFromSceneFile() const
{
	const auto sceneFilePath = m_args.getSceneFilePath();

	return phLoadCommands(m_engineId, sceneFilePath.c_str()) == PH_TRUE ? true : false;
}

}// end namespace ph::cli
