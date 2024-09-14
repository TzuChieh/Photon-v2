#include "StaticImageRenderer.h"

#include <filesystem>

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

	// Use the directory of the scene file as working directory
	namespace fs = std::filesystem;
	const std::string sceneDirectory = fs::path(path).parent_path().string();
	phSetWorkingDirectory(m_engineId, sceneDirectory.c_str());
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
