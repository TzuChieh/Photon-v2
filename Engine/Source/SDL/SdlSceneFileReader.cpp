#include "SDL/SdlSceneFileReader.h"
#include "ph_cpp_core.h"
#include "SDL/SceneDescription.h"
#include "DataIO/Stream/FormattedTextInputStream.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "Utility/utility.h"
#include "SDL/sdl_helpers.h"
#include "Utility/Timer.h"

#include <utility>
#include <memory>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlSceneFileReader, SDL);

SdlSceneFileReader::SdlSceneFileReader()
	: SdlSceneFileReader("untitled-scene", Path("./temp_sdl/"))
{}

SdlSceneFileReader::SdlSceneFileReader(std::string sceneName, const Path& sceneWorkingDirectory)
	: SdlCommandParser(get_registered_engine_classes(), sceneWorkingDirectory)
	, m_sceneName(std::move(sceneName))
	, m_fileStream(nullptr)
	, m_scene(nullptr)
{}

SdlSceneFileReader::~SdlSceneFileReader() = default;

bool SdlSceneFileReader::beginCommand(ESdlCommandType /* commandType */, const SdlClass* /* targetClass */)
{
	// Consume all commands
	return true;
}

ISdlResource* SdlSceneFileReader::createResource(
	std::string_view resourceName,
	const SdlClass* const resourceClass,
	const ESdlCommandType commandType)
{
	if(!resourceClass || !resourceClass->allowCreateFromClass())
	{
		PH_LOG_WARNING(SdlSceneFileReader,
			"Unable to create resource {} (class = {}): {}",
			resourceName, 
			sdl::gen_pretty_name(resourceClass), 
			!resourceClass ? "null class" : "not allowed to be created from class");
		return nullptr;
	}

	std::shared_ptr<ISdlResource> resource = resourceClass->createResource();

	// Add the resource to storage
	if(resource && m_scene)
	{
		// TODO: use string_view
		if(commandType == ESdlCommandType::Load)
		{
			m_scene->getResources().add(resource, std::string(resourceName));
		}
		else if(commandType == ESdlCommandType::Phantom)
		{
			m_scene->getPhantoms().add(resource, std::string(resourceName));
		}
		else
		{
			PH_LOG_WARNING(SdlSceneFileReader, 
				"Resource {} (class = {}) generated for command type {} and is ignored",
				resourceName, 
				sdl::gen_pretty_name(resourceClass), 
				enum_to_value(commandType));
			resource = nullptr;
		}
	}

	return resource.get();
}

void SdlSceneFileReader::initResource(
	ISdlResource* const resource,
	const SdlClass* const resourceClass,
	std::string_view resourceName,
	SdlInputClauses& clauses,
	const ESdlCommandType commandType)
{
	if(!resource || !resourceClass)
	{
		PH_LOG_WARNING(SdlSceneFileReader,
			"Unable to initialize resource {} (class = {}): {}",
			resourceName,
			sdl::gen_pretty_name(resourceClass),
			!resource ? "null resource" : "null class");
		return;
	}

	// TODO: reuse input ctx
	SdlInputContext inputContext(m_scene, getSceneWorkingDirectory(), resourceClass);
	resourceClass->initResource(*resource, clauses, inputContext);
}

ISdlResource* SdlSceneFileReader::getResource(std::string_view resourceName, const ESdlTypeCategory category)
{
	if(!m_scene)
	{
		return nullptr;
	}

	// TODO: use string_view
	// TODO: just get resource ptr, not shared_ptr
	std::shared_ptr<ISdlResource> resource = m_scene->getResources().get(std::string(resourceName));
	return resource.get();
}

void SdlSceneFileReader::runExecutor(
	std::string_view executorName,
	const SdlClass* const targetClass,
	ISdlResource* const targetResource,
	SdlInputClauses& clauses,
	const ESdlCommandType commandType)
{
	if(!targetClass)
	{
		PH_LOG_WARNING(SdlSceneFileReader,
			"Unable to run executor {}: {}",
			executorName, "null class");
		return;
	}

	// TODO: reuse input ctx
	SdlInputContext inputContext(m_scene, getSceneWorkingDirectory(), targetClass);
	targetClass->call(executorName, targetResource, clauses, inputContext);
}

void SdlSceneFileReader::commandVersionSet(const SemanticVersion& version)
{}

void SdlSceneFileReader::endCommand()
{}

void SdlSceneFileReader::read(SceneDescription* const scene)
{
	// Only update current scene if `scene` is not null
	if(scene)
	{
		setScene(scene);
	}

	if(m_scene)
	{
		m_scene->setWorkingDirectory(getSceneWorkingDirectory());
	}
	else
	{
		PH_LOG_WARNING(SdlSceneFileReader,
			"Unable to read scene {} (from {}): no scene description is given",
			m_sceneName, getSceneWorkingDirectory());
		return;
	}

	// Scene file must reside in the scene working directory as it may be accompanied with data files
	Path filePath = getSceneWorkingDirectory().append(m_sceneName + ".p2");

	FormattedTextInputStream commandFile(filePath);
	if(!commandFile)
	{
		PH_LOG_WARNING(SdlSceneFileReader,
			"command file <{}> opening failed", 
			filePath.toAbsoluteString());
		return;
	}
	else
	{
		PH_LOG(SdlSceneFileReader, 
			"loading command file <{}>", 
			filePath.toAbsoluteString());

		Timer timer;
		timer.start();

		std::string lineOfCommand;
		while(commandFile)
		{
			commandFile.readLine(&lineOfCommand);
			lineOfCommand += '\n';

			parse(lineOfCommand);
		}

		timer.stop();

		PH_LOG(SdlSceneFileReader, 
			"command file PSDL version: {}", getCommandVersion().toString());
		PH_LOG(SdlSceneFileReader, 
			"command file loaded, time elapsed = {} ms", timer.getDeltaMs());
	}
}

void SdlSceneFileReader::setSceneName(std::string sceneName)
{
	m_sceneName = std::move(sceneName);
}

void SdlSceneFileReader::setScene(SceneDescription* const scene)
{
	m_scene = scene;
}

}// end namespace ph
