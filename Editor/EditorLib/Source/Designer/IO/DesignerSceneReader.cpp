#include "Designer/IO/DesignerSceneReader.h"
#include "ph_editor.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"
#include "Designer/IO/DesignerDataPacketInterface.h"

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
	
	, m_sceneName()
	, m_scene(nullptr)
	, m_metaInfo()
	, m_nameToNewObjs()
{
	setPacketInterface(std::make_unique<DesignerDataPacketInterface>());
}

void DesignerSceneReader::read()
{
	if(!m_sceneName.empty() && m_scene)
	{
		m_scene->setWorkingDirectory(getSceneWorkingDirectory());
	}
	else
	{
		PH_LOG(DesignerSceneReader, Warning,
			"Unable to read designer scene from {}: target designer scene info incomplete "
			"(name={}, storage={})", getSceneWorkingDirectory(), m_sceneName, static_cast<void*>(m_scene));
		return;
	}

	// Read meta info before scene (the scene loading process depends on it)
	readSceneMetaInfo();

	readScene();
}

void DesignerSceneReader::setSceneInfo(std::string sceneName, DesignerScene* scene)
{
	if(m_sceneName != sceneName || m_scene != scene)
	{
		// Also clear meta info if scene changed
		m_metaInfo = DesignerSceneMetaInfo{};
	}

	m_sceneName = sceneName;
	m_scene = scene;
}

bool DesignerSceneReader::beginCommand(
	const ESdlCommandType commandType,
	const SdlClass* const targetClass,
	SdlInputContext* const out_ctx)
{
	if(commandType == ESdlCommandType::Phantom || commandType == ESdlCommandType::NamedDataPacket)
	{
		PH_LOG(DesignerSceneReader, Warning,
			"Phantom and named data packet commands are not supported (class = {}), "
			"ignoring this command", sdl::gen_pretty_name(targetClass));
		return false;
	}

	*out_ctx = SdlInputContext(&getSceneWorkingDirectory(), targetClass);

	// Consume all commands
	return true;
}

void DesignerSceneReader::endCommand()
{}

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
		PH_LOG(DesignerSceneReader, Warning,
			"Designer object {} (class = {}) meta info missing, creating as root={}",
			resourceName, sdl::gen_pretty_name(resourceClass), isRootObj);
	}

	// Object created do not need to be pre-initialized in any way--their data will be populated by
	// SDL clauses (see `initResource()`) and later initialized w.r.t. scene in bulk (see `readScene()`)
	DesignerObject* newObj = nullptr;
	if(isRootObj)
	{
		newObj = m_scene->newRootObject(resourceClass, false, false);
	}
	else
	{
		newObj = m_scene->newObject(resourceClass, false, false);
	}

	if(m_nameToNewObjs.contains(resourceName))
	{
		PH_LOG(DesignerSceneReader, Warning,
			"Duplicated designer object {} (class = {}) found, overwriting",
			resourceName, sdl::gen_pretty_name(resourceClass));
	}

	m_nameToNewObjs[std::string(resourceName)] = newObj;
	return newObj;
}

void DesignerSceneReader::initResource(
	std::string_view resourceName,
	ISdlResource* const resource,
	const SdlInputContext& ctx,
	SdlInputClauses& clauses,
	ESdlCommandType /* commandType */)
{
	const SdlClass* resourceClass = ctx.getSrcClass();
	if(!resource || !resourceClass)
	{
		PH_LOG(DesignerSceneReader, Error,
			"Unable to initialize designer resource {} (class = {}): {}",
			resourceName,
			sdl::gen_pretty_name(resourceClass),
			!resource ? "null resource" : "null class");
		return;
	}

	// Load saved data into resource instance
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

	PH_LOG(DesignerSceneReader, Warning,
		"Attempting to run executor {} (class = {}): calling executor is not supported",
		executorName, sdl::gen_pretty_name(ctx.getSrcClass()));
}

void DesignerSceneReader::commandVersionSet(
	const SemanticVersion& /* version */,
	const SdlInputContext& /* ctx */)
{}

void DesignerSceneReader::storeNamedDataPacket(
	std::string_view packetName,
	const SdlInputClauses& packet,
	const SdlInputContext& ctx)
{
	PH_ASSERT_UNREACHABLE_SECTION();
}

void DesignerSceneReader::readScene()
{
	PH_ASSERT(m_scene);

	// Scene file must reside in the root of scene working directory as it will be accompanied
	// with additional data files
	Path filePath = getSceneWorkingDirectory().append(m_sceneName + ".pds");

	FormattedTextInputStream commandFile(filePath);
	if(!commandFile)
	{
		PH_LOG(DesignerSceneReader, Warning,
			"Command file <{}> opening failed.", filePath.toAbsoluteString());
		return;
	}
	else
	{
		PH_LOG(DesignerSceneReader, Note,
			"Loading command file <{}>...", filePath.toAbsoluteString());

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

		PH_LOG(DesignerSceneReader, Note,
			"Command file loaded (PSDL version: {}), time elapsed = {} ms.", 
			getCommandVersion().toString(), timer.getDeltaMs());
	}

	flush();

	// Initialize all newly created designer objects w.r.t. scene
	for(auto&& [name, newObj] : m_nameToNewObjs)
	{
		m_scene->initObject(newObj);
	}

	// Establish object states according to stored meta info
	for(auto&& [name, newObj] : m_nameToNewObjs)
	{
		auto* const objMetaInfo = m_metaInfo.getObject(name);
		if(!objMetaInfo)
		{
			continue;
		}

		if(!objMetaInfo->isRoot())
		{
			auto findResult = m_nameToNewObjs.find(objMetaInfo->parentName);
			if(findResult != m_nameToNewObjs.end())
			{
				DesignerObject* parent = findResult->second;
				parent->addNewChild(newObj);
			}
			else
			{
				PH_LOG(DesignerSceneReader, Warning,
					"Designer object {} cannot find its parent {}",
					name, objMetaInfo->parentName);
			}
		}

		if(objMetaInfo->isTicking)
		{
			newObj->setTick(true);
		}

		if(objMetaInfo->isRenderTicking)
		{
			newObj->setRenderTick(true);
		}
	}

	// We are done processing newly created objects
	m_nameToNewObjs.clear();
}

void DesignerSceneReader::readSceneMetaInfo()
{
	m_metaInfo.load(getSceneWorkingDirectory(), m_sceneName);
}

}// end namespace ph::editor
