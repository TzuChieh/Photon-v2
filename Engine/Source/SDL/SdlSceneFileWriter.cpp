#include "SDL/SdlSceneFileWriter.h"
#include "DataIO/Stream/IOutputStream.h"
#include "Common/assertion.h"
#include "Common/config.h"
#include "Common/logging.h"
#include "DataIO/Stream/FormattedTextFileOutputStream.h"

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

void SdlSceneFileWriter::generatedCommand(std::string_view commandStr)
{
	PH_ASSERT(m_fileStream);
	m_fileStream->writeString(commandStr);
}

bool SdlSceneFileWriter::endCommand()
{
	return true;
}

void SdlSceneFileWriter::save(const SceneDescription& scene)
{
	// TODO: currently will overwrite existing file; should provide options for whether to append

	// Scene file resides in a folder with the same name as it may be accompanied with data files
	Path sceneFilePath = getSceneWorkingDirectory().append(m_sceneName);
	sceneFilePath.createDirectory();
	sceneFilePath.append(m_sceneName + ".p2");

	FormattedTextFileOutputStream fileStream(sceneFilePath);
	m_fileStream = &fileStream;
	m_fileStream->writeString("#version {};\n", PH_PSDL_VERSION);

	generateScene(scene);

	PH_LOG(SdlSceneFileWriter, "scene file written to {}", sceneFilePath);
}

}// end namespace ph
