#include "SDL/SdlSceneFileWriter.h"
#include "DataIO/Stream/IOutputStream.h"
#include "Common/assertion.h"
#include "Common/config.h"
#include "Common/logging.h"
#include "DataIO/Stream/FormattedTextOutputStream.h"
#include "SDL/sdl_helpers.h"
#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/Introspect/SdlClass.h"
#include "ph_cpp_core.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlSceneFileWriter, SDL);

SdlSceneFileWriter::SdlSceneFileWriter()
	: SdlSceneFileWriter("untitled-scene", Path("./"))
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
	Path sceneFilePath = getSceneWorkingDirectory().append(m_sceneName + ".p2");

	PH_LOG(SdlSceneFileWriter, "generating scene file: {}", sceneFilePath);

	clearStats();

	FormattedTextOutputStream fileStream(sceneFilePath);
	m_fileStream = &fileStream;
	m_fileStream->writeString("#version {};\n", PH_PSDL_VERSION);

	m_resolver.analyze(scene);

	for(const ISdlResource* resource = m_resolver.next();
	    resource != nullptr; 
	    resource = m_resolver.next())
	{
		const SdlClass* clazz = resource->getDynamicSdlClass();
		PH_ASSERT(clazz);

		generateLoadCommand(resource, m_resolver.getResourceName(resource));
	}

	PH_LOG(SdlSceneFileWriter,
		"generated {} commands (errors: {})", 
		numGeneratedCommands(), numGenerationErrors());

	PH_LOG(SdlSceneFileWriter, "scene file generated");
	m_fileStream = nullptr;
}

void SdlSceneFileWriter::setSceneName(std::string sceneName)
{
	m_sceneName = std::move(sceneName);
}

}// end namespace ph
