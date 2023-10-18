#include "SDL/SdlSceneFileWriter.h"
#include "Common/assertion.h"
#include "Common/config.h"
#include "Common/logging.h"
#include "DataIO/Stream/FormattedTextOutputStream.h"
#include "SDL/sdl_helpers.h"
#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/Introspect/SdlClass.h"
#include "DataIO/FileSystem/Filesystem.h"
#include "ph_cpp_core.h"
#include "SDL/SceneDescription.h"
#include "Utility/SemanticVersion.h"

#include <utility>
#include <vector>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlSceneFileWriter, SDL);

SdlSceneFileWriter::SdlSceneFileWriter()
	: SdlSceneFileWriter(Path("untitled-scene.p2"), Path("./temp_sdl/"))
{}

SdlSceneFileWriter::SdlSceneFileWriter(const Path& sceneFile, const Path& sceneWorkingDirectory)
	: SdlSceneFileWriter(
		get_registered_engine_classes(), 
		sceneFile,
		sceneWorkingDirectory)
{}

SdlSceneFileWriter::SdlSceneFileWriter(
	TSpanView<const SdlClass*> targetClasses,
	const Path& sceneFile,
	const Path& sceneWorkingDirectory)

	: SdlCommandGenerator(targetClasses, sceneWorkingDirectory)

	, m_sceneFile(sceneFile)
	, m_resolver()
	, m_fileStream(nullptr)
{}

SdlSceneFileWriter::~SdlSceneFileWriter() = default;

bool SdlSceneFileWriter::beginCommand(
	const SdlClass* const targetClass,
	SdlOutputContext* const out_ctx)
{
	*out_ctx = SdlOutputContext(&m_resolver, getSceneWorkingDirectory(), targetClass);

	return true;
}

void SdlSceneFileWriter::endCommand()
{}

void SdlSceneFileWriter::saveResource(
	const ISdlResource* const resource,
	const SdlOutputContext& ctx,
	SdlOutputClauses& clauses)
{
	const SdlClass* resourceClass = ctx.getSrcClass();
	if(!resource || !resourceClass)
	{
		PH_LOG_WARNING(SdlSceneFileWriter,
			"Unable to save resource (class = {}): {}",
			sdl::gen_pretty_name(resourceClass),
			!resource ? "null resource" : "null class");
		return;
	}

	resourceClass->saveResource(*resource, clauses, ctx);
}

void SdlSceneFileWriter::commandGenerated(
	std::string_view commandStr,
	const SdlOutputContext& /* ctx */)
{
	if(!m_fileStream)
	{
		return;
	}

	m_fileStream->writeString(commandStr);
}

void SdlSceneFileWriter::write(const SceneDescription& scene)
{
	// TODO: currently will overwrite existing file; should provide options for whether to append

	PH_LOG(SdlSceneFileWriter, "generating scene file: {}", m_sceneFile);

	Filesystem::createDirectories(getSceneWorkingDirectory());
	clearStats();
	saveSceneToFile(scene);

	PH_LOG(SdlSceneFileWriter,
		"scene file generated, totalling {} commands (errors: {})",
		numGeneratedCommands(), numGenerationErrors());
}

void SdlSceneFileWriter::setSceneFile(Path sceneFile)
{
	m_sceneFile = std::move(sceneFile);
}

void SdlSceneFileWriter::saveSceneToFile(const SceneDescription& scene)
{
	FormattedTextOutputStream fileStream(m_sceneFile);
	m_fileStream = &fileStream;

	generateVersionCommand(SemanticVersion(PH_PSDL_VERSION));

	std::vector<std::string> names;
	std::vector<const ISdlResource*> resources = scene.getResources().listAll(&names);
	m_resolver.analyze(resources, names);

	for(const ISdlResource* resource = m_resolver.next();
	    resource != nullptr; 
	    resource = m_resolver.next())
	{
		generateLoadCommand(resource, m_resolver.getResourceName(resource));
	}

	m_fileStream = nullptr;
}

}// end namespace ph
