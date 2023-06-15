#include "SDL/SdlSceneFileWriter.h"
#include "Common/assertion.h"
#include "Common/config.h"
#include "Common/logging.h"
#include "DataIO/Stream/FormattedTextOutputStream.h"
#include "SDL/sdl_helpers.h"
#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/Introspect/SdlClass.h"
#include "DataIO/FileSystem/Path.h"
#include "ph_cpp_core.h"
#include "SDL/SceneDescription.h"
#include "Utility/SemanticVersion.h"

#include <utility>
#include <vector>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlSceneFileWriter, SDL);

SdlSceneFileWriter::SdlSceneFileWriter()
	: SdlSceneFileWriter("untitled-scene", Path("./temp_sdl/"))
{}

SdlSceneFileWriter::SdlSceneFileWriter(std::string sceneName, const Path& sceneWorkingDirectory)
	: SdlCommandGenerator(get_registered_engine_classes(), sceneWorkingDirectory)
	, m_sceneName(std::move(sceneName))
	, m_resolver()
	, m_fileStream(nullptr)
{}

SdlSceneFileWriter::~SdlSceneFileWriter() = default;

bool SdlSceneFileWriter::beginCommand(const SdlClass* const targetClass)
{
	return true;
}

void SdlSceneFileWriter::saveResource(
	const ISdlResource* const resource,
	const SdlClass* const resourceClass,
	SdlOutputClauses& clauses)
{
	if(!resource || !resourceClass)
	{
		PH_LOG_WARNING(SdlSceneFileWriter,
			"Unable to save resource (class = {}): {}",
			sdl::gen_pretty_name(resourceClass),
			!resource ? "null resource" : "null class");
		return;
	}

	// TODO: reuse output ctx
	SdlOutputContext outputContext(&m_resolver, getSceneWorkingDirectory(), resourceClass);
	resourceClass->saveResource(*resource, clauses, outputContext);
}

void SdlSceneFileWriter::commandGenerated(std::string_view commandStr)
{
	if(!m_fileStream)
	{
		return;
	}

	m_fileStream->writeString(commandStr);
}

void SdlSceneFileWriter::endCommand()
{}

void SdlSceneFileWriter::write(const SceneDescription& scene)
{
	// TODO: currently will overwrite existing file; should provide options for whether to append

	// Scene file must reside in the scene working directory as it may be accompanied with data files
	getSceneWorkingDirectory().createDirectory();
	Path sceneFile = getSceneWorkingDirectory().append(m_sceneName + ".p2");

	PH_LOG(SdlSceneFileWriter, "generating scene file: {}", sceneFile);

	clearStats();
	saveSceneToFile(scene, sceneFile);

	PH_LOG(SdlSceneFileWriter,
		"scene file generated, totalling {} commands (errors: {})",
		numGeneratedCommands(), numGenerationErrors());
}

void SdlSceneFileWriter::setSceneName(std::string sceneName)
{
	m_sceneName = std::move(sceneName);
}

void SdlSceneFileWriter::saveSceneToFile(const SceneDescription& scene, const Path& filePath)
{
	FormattedTextOutputStream fileStream(filePath);
	m_fileStream = &fileStream;

	generateVersionCommand(SemanticVersion(PH_PSDL_VERSION));

	std::vector<const ISdlResource*> resources;
	std::vector<std::string_view> names;
	scene.getResources().listAll(resources, &names);

	m_resolver.analyze(resources, names);

	for(const ISdlResource* resource = m_resolver.next();
	    resource != nullptr; 
	    resource = m_resolver.next())
	{
		const SdlClass* clazz = resource->getDynamicSdlClass();
		PH_ASSERT(clazz);

		generateLoadCommand(resource, m_resolver.getResourceName(resource));
	}

	m_fileStream = nullptr;
}

}// end namespace ph
