#include "StaticImageRenderer.h"

#include <filesystem>

namespace ph::cli
{

StaticImageRenderer::StaticImageRenderer(const ProcessedArguments& args)
	: m_sessionId(0)
	, m_args(args)
{
	phCreateSession(&m_sessionId, static_cast<PhUInt32>(args.numThreads()));
}

StaticImageRenderer::~StaticImageRenderer()
{
	phDeleteSession(m_sessionId);
}

void StaticImageRenderer::setSceneFilePath(const std::string& path)
{
	m_args.setSceneFilePath(path);

	// Use the directory of the scene file as working directory
	namespace fs = std::filesystem;
	const std::string sceneDirectory = fs::path(path).parent_path().string();
	phSetWorkingDirectory(m_sessionId, sceneDirectory.c_str());
}

void StaticImageRenderer::setImageOutputStem(const std::string& imageOutputStem)
{
	m_args.setImageOutputStem(imageOutputStem);
}

bool StaticImageRenderer::loadCommandsFromSceneFile() const
{
	const auto sceneFilePath = m_args.getSceneFilePath();

	return phLoadCommands(m_sessionId, sceneFilePath.c_str()) == PH_OK;
}

}// end namespace ph::cli
