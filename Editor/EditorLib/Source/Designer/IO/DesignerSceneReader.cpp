#include "Designer/IO/DesignerSceneReader.h"
#include "ph_editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"

#include <SDL/Introspect/SdlInputContext.h>
#include <Common/logging.h>
#include <SDL/sdl_helpers.h>
#include <DataIO/Stream/FormattedTextInputStream.h>
#include <Utility/Timer.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(DesignerSceneReader, Designer);

DesignerSceneReader::DesignerSceneReader()
	: DesignerSceneReader(Path("./temp_sdl/"))
{}

DesignerSceneReader::DesignerSceneReader(const Path& sceneWorkingDirectory)
	: SdlCommandParser(get_registered_editor_classes(), sceneWorkingDirectory)
	, m_designerPacketInterface()
	, m_scene(nullptr)
	, m_metaInfo()
{}

DesignerSceneReader::~DesignerSceneReader() = default;

SdlDataPacketInterface& DesignerSceneReader::getPacketInterface()
{
	return m_designerPacketInterface;
}

void DesignerSceneReader::read(DesignerScene* const scene)
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
		PH_LOG_WARNING(DesignerSceneReader,
			"Unable to read designer scene from {}: no target designer scene was set",
			getSceneWorkingDirectory());
		return;
	}

	// Read meta info before scene (the scene loading process depends on it)
	readSceneMetaInfo();

	readScene();
}

void DesignerSceneReader::setScene(DesignerScene* const scene)
{
	if(m_scene != scene)
	{
		// Also clear meta info if scene changed
		m_metaInfo = DesignerSceneMetaInfo();
	}

	m_scene = scene;
}

bool DesignerSceneReader::beginCommand(
	const ESdlCommandType commandType,
	const SdlClass* const targetClass,
	SdlInputContext* const out_ctx)
{
	if(commandType == ESdlCommandType::Phantom)
	{
		PH_LOG_WARNING(DesignerSceneReader,
			"Phantom command should not be used to create designer resource (class = {}), "
			"ignoring this command",
			sdl::gen_pretty_name(targetClass));
		return false;
	}

	*out_ctx = SdlInputContext(getSceneWorkingDirectory(), targetClass);

	// Consume all commands
	return true;
}

ISdlResource* DesignerSceneReader::createResource(
	std::string_view resourceName,
	const SdlInputContext& ctx,
	const ESdlCommandType commandType)
{
	if(!m_scene)
	{
		return nullptr;
	}

	// A special case: designer scene is the one specified as reader's input
	const SdlClass* resourceClass = ctx.getSrcClass();
	if(resourceClass == m_scene->getDynamicSdlClass())
	{
		return m_scene;
	}

	// Remaining cases should all be designer objects: they are managed by their parent scene, 
	// need to be created by designer scene

	auto* const objMetaInfo = m_metaInfo.getObject(resourceName);

	bool isRootObj = true;
	if(objMetaInfo)
	{
		isRootObj = objMetaInfo->isRoot();
	}
	else
	{
		PH_LOG_WARNING(DesignerSceneReader,
			"Designer object {} (class = {}) meta info missing, creating as root={}",
			resourceName,
			sdl::gen_pretty_name(resourceClass),
			isRootObj);
	}

	// Object created do not need to be pre-initialized in any way--they will be initialized by
	// SDL clauses (see `initResource()`)
	if(isRootObj)
	{
		return m_scene->newRootObject(resourceClass, false, false);
	}
	else
	{
		return m_scene->newObject(resourceClass, false, false);
	}
}

void DesignerSceneReader::initResource(
	ISdlResource* const resource,
	const SdlInputContext& ctx,
	std::string_view resourceName,
	SdlInputClauses& clauses,
	ESdlCommandType /* commandType */)
{
	const SdlClass* resourceClass = ctx.getSrcClass();
	if(!resource || !resourceClass)
	{
		PH_LOG_WARNING(DesignerSceneReader,
			"Unable to initialize designer resource {} (class = {}): {}",
			resourceName,
			sdl::gen_pretty_name(resourceClass),
			!resource ? "null resource" : "null class");
		return;
	}

	resourceClass->initResource(*resource, clauses, ctx);
}

ISdlResource* DesignerSceneReader::getResource(
	std::string_view resourceName,
	const SdlInputContext& ctx)
{
	if(!m_scene)
	{
		return nullptr;
	}

	return m_scene->findObjectByName(resourceName);
}

void DesignerSceneReader::runExecutor(
	std::string_view executorName,
	const SdlInputContext& ctx,
	ISdlResource* /* targetResource */,
	SdlInputClauses& /* clauses */,
	ESdlCommandType /* commandType */)
{
	// Running executor is not a feature supported by designer scene

	PH_LOG_WARNING(DesignerSceneReader,
		"Attempting to run executor {} (class = {}): calling executor is not supported",
		executorName, sdl::gen_pretty_name(ctx.getSrcClass()));
}

void DesignerSceneReader::commandVersionSet(
	const SemanticVersion& /* version */,
	const SdlInputContext& /* ctx */)
{}

void DesignerSceneReader::endCommand()
{}

void DesignerSceneReader::readScene()
{
	PH_ASSERT(m_scene);

	// Scene file must reside in the scene working directory as it may be accompanied with data files
	Path filePath = getSceneWorkingDirectory().append(m_scene->getName() + ".pds");

	FormattedTextInputStream commandFile(filePath);
	if(!commandFile)
	{
		PH_LOG_WARNING(DesignerSceneReader,
			"command file <{}> opening failed", 
			filePath.toAbsoluteString());
		return;
	}
	else
	{
		PH_LOG(DesignerSceneReader,
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

		PH_LOG(DesignerSceneReader,
			"command file PSDL version: {}", getCommandVersion().toString());
		PH_LOG(DesignerSceneReader,
			"command file loaded, time elapsed = {} ms", timer.getDeltaMs());
	}

	// TODO: tick, child from meta info
}

void DesignerSceneReader::readSceneMetaInfo()
{
	PH_ASSERT(m_scene);

	m_metaInfo.load(getSceneWorkingDirectory(), m_scene->getName());
}

}// end namespace ph::editor
