#include "SDL/SdlSceneFileWriter.h"
#include "DataIO/Stream/IOutputStream.h"
#include "Common/assertion.h"
#include "Common/config.h"
#include "Common/logging.h"
#include "DataIO/Stream/FormattedTextOutputStream.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlSceneFileWriter, SDL);

SdlSceneFileWriter::SdlSceneFileWriter()
	: SdlSceneFileWriter("untitled-scene", Path("./"))
{}

SdlSceneFileWriter::SdlSceneFileWriter(std::string sceneName, Path sceneWorkingDirectory)
	: SdlCommandGenerator(std::move(sceneWorkingDirectory))
	, m_sceneName(std::move(sceneName))
	, m_fileStream(nullptr)
{}

SdlSceneFileWriter::~SdlSceneFileWriter() = default;

bool SdlSceneFileWriter::beginCommand(const SdlClass* const targetClass)
{
	return true;
}

void SdlSceneFileWriter::commandGenerated(std::string_view commandStr)
{
	PH_ASSERT(m_fileStream);
	m_fileStream->writeString(commandStr);
}

void SdlSceneFileWriter::endCommand()
{}

void SdlSceneFileWriter::write(const SceneDescription& scene)
{
	// TODO: currently will overwrite existing file; should provide options for whether to append

	// Scene file must reside in the scene working directory as it may be accompanied with data files
	getSceneWorkingDirectory().createDirectory();
	Path sceneFilePath = getSceneWorkingDirectory().append(m_sceneName + ".p2");

	FormattedTextOutputStream fileStream(sceneFilePath);
	m_fileStream = &fileStream;
	m_fileStream->writeString("#version {};\n", PH_PSDL_VERSION);

	generateScene(scene);

	PH_LOG(SdlSceneFileWriter, "scene file written to {}", sceneFilePath);
	m_fileStream = nullptr;
}

void SdlSceneFileWriter::setSceneName(std::string sceneName)
{
	m_sceneName = std::move(sceneName);
}

}// end namespace ph
