#include "Designer/IO/DesignerSceneWriter.h"
#include "ph_editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/Stream/FormattedTextOutputStream.h>
#include <SDL/sdl_helpers.h>
#include <SDL/Introspect/SdlOutputContext.h>
#include <SDL/Introspect/SdlClass.h>
#include <Utility/SemanticVersion.h>
#include <Common/config.h>

#include <utility>
#include <vector>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(DesignerSceneWriter, Designer);

DesignerSceneWriter::DesignerSceneWriter()
	: DesignerSceneWriter(Path("./"))
{}

DesignerSceneWriter::DesignerSceneWriter(const Path& sceneWorkingDirectory)

	: SdlCommandGenerator(get_registered_editor_classes(), sceneWorkingDirectory)

	, m_resolver()
	, m_packetInterface(sceneWorkingDirectory)
	, m_fileStream(nullptr)
{}

DesignerSceneWriter::~DesignerSceneWriter() = default;

SdlDataPacketInterface& DesignerSceneWriter::getPacketInterface()
{
	return m_packetInterface;
}

bool DesignerSceneWriter::beginCommand(const SdlClass* const targetClass)
{
	return true;
}

void DesignerSceneWriter::saveResource(
	const ISdlResource* const resource,
	const SdlClass* const resourceClass,
	SdlOutputClauses& clauses)
{
	if(!resource || !resourceClass)
	{
		PH_LOG_WARNING(DesignerSceneWriter,
			"Unable to save resource (class = {}): {}",
			sdl::gen_pretty_name(resourceClass),
			!resource ? "null resource" : "null class");
		return;
	}

	// TODO: reuse output ctx
	SdlOutputContext outputContext(&m_resolver, getSceneWorkingDirectory(), resourceClass);
	resourceClass->saveResource(*resource, clauses, outputContext);
}

void DesignerSceneWriter::commandGenerated(std::string_view commandStr)
{
	if(!m_fileStream)
	{
		return;
	}

	m_fileStream->writeString(commandStr);
}

void DesignerSceneWriter::endCommand()
{}

void DesignerSceneWriter::write(const DesignerScene& scene)
{
	// TODO: currently will overwrite existing file; should provide options for whether to append

	if(!scene.isPaused())
	{
		PH_LOG_WARNING(DesignerSceneWriter,
			"Designer scene {} should be paused before writing started. If not, its internal states "
			"might change and cause corruption in the saved data.",
			scene.getName());
	}

	// Expected be set, too late to determine a good link here
	PH_ASSERT(!scene.getRenderDescriptionLink().isEmpty());

	// Scene file must reside in the scene working directory as it may be accompanied with data files
	getSceneWorkingDirectory().createDirectory();
	Path sceneFile = getSceneWorkingDirectory().append(scene.getName() + ".pds");

	PH_LOG(DesignerSceneWriter, "generating scene file: {}", sceneFile);

	clearStats();
	saveSceneToFile(scene, sceneFile);

	PH_LOG(DesignerSceneWriter,
		"scene file generated, totalling {} commands (errors: {})", 
		numGeneratedCommands(), numGenerationErrors());
}

void DesignerSceneWriter::saveSceneToFile(const DesignerScene& scene, const Path& filePath)
{
	FormattedTextOutputStream fileStream(filePath);
	m_fileStream = &fileStream;

	generateVersionCommand(SemanticVersion(PH_PSDL_VERSION));

	// Find by the common base type `DesignerObject` effectively retrieves all valid objects
	std::vector<DesignerObject*> objs;
	scene.findObjectsByType(objs);

	std::vector<std::string_view> names;
	names.reserve(objs.size());
	for(DesignerObject* obj : objs)
	{
		names.push_back(obj->getName());
	}

	std::vector<ISdlResource*> resources(objs.begin(), objs.end());
	m_resolver.analyze(resources, names);

	for(const ISdlResource* resource = m_resolver.next();
	    resource != nullptr; 
	    resource = m_resolver.next())
	{
		const SdlClass* clazz = resource->getDynamicSdlClass();
		PH_ASSERT(clazz);

		generateLoadCommand(resource, m_resolver.getResourceName(resource));
	}

	generateLoadCommand(&scene, scene.getName());

	m_fileStream = nullptr;
}

}// end namespace ph::editor
