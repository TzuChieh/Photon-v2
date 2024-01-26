#include "SDL/SdlSceneFileReader.h"
#include "ph_core.h"
#include "SDL/SceneDescription.h"
#include "DataIO/Stream/FormattedTextInputStream.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "Utility/utility.h"
#include "SDL/sdl_helpers.h"
#include "Utility/Timer.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>
#include <memory>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlSceneFileReader, SDL);

SdlSceneFileReader::SdlSceneFileReader()
	: SdlSceneFileReader(Path("untitled-scene.p2"), Path("./temp_sdl/"))
{}

SdlSceneFileReader::SdlSceneFileReader(const Path& sceneFile, const Path& sceneWorkingDirectory)
	: SdlSceneFileReader(
		get_registered_engine_classes(), 
		sceneFile,
		sceneWorkingDirectory)
{}

SdlSceneFileReader::SdlSceneFileReader(
	TSpanView<const SdlClass*> targetClasses,
	const Path& sceneFile,
	const Path& sceneWorkingDirectory)

	: SdlCommandParser(targetClasses, sceneWorkingDirectory)

	, m_sceneFile(sceneFile)
	, m_namedDataPackets()
	, m_scene(nullptr)
{}

SdlSceneFileReader::~SdlSceneFileReader() = default;

bool SdlSceneFileReader::beginCommand(
	ESdlCommandType /* commandType */, 
	const SdlClass* const targetClass,
	SdlInputContext* const out_ctx)
{
	*out_ctx = SdlInputContext(m_scene, &m_namedDataPackets, &getSceneWorkingDirectory(), targetClass);

	// Consume all commands
	return true;
}

void SdlSceneFileReader::endCommand()
{}

ISdlResource* SdlSceneFileReader::createResource(
	std::string_view resourceName,
	const SdlInputContext& ctx,
	const ESdlCommandType commandType)
{
	const SdlClass* resourceClass = ctx.getSrcClass();
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
	std::string_view resourceName,
	ISdlResource* const resource,
	const SdlInputContext& ctx,
	SdlInputClauses& clauses,
	const ESdlCommandType /* commandType */)
{
	const SdlClass* resourceClass = ctx.getSrcClass();
	if(!resource || !resourceClass)
	{
		PH_LOG_WARNING(SdlSceneFileReader,
			"Unable to initialize resource {} (class = {}): {}",
			resourceName,
			sdl::gen_pretty_name(resourceClass),
			!resource ? "null resource" : "null class");
		return;
	}

	resourceClass->initResource(*resource, clauses, ctx);
}

ISdlResource* SdlSceneFileReader::getResource(
	std::string_view resourceName, 
	const SdlInputContext& /* ctx */)
{
	if(!m_scene)
	{
		return nullptr;
	}

	// TODO: just get resource ptr, not shared_ptr
	std::shared_ptr<ISdlResource> resource = m_scene->get(resourceName);
	return resource.get();
}

void SdlSceneFileReader::runExecutor(
	std::string_view executorName,
	const SdlInputContext& ctx,
	ISdlResource* const targetResource,
	SdlInputClauses& clauses,
	const ESdlCommandType commandType)
{
	// Note that we do not obtain target class from resource since an executor may be called with
	// an explicit class (context object has this information)
	const SdlClass* targetClass = ctx.getSrcClass();
	if(!targetClass)
	{
		PH_LOG_WARNING(SdlSceneFileReader,
			"Unable to run executor {}: null target class",
			executorName);
		return;
	}

	targetClass->call(executorName, targetResource, clauses, ctx);
}

void SdlSceneFileReader::commandVersionSet(
	const SemanticVersion& /* version */,
	const SdlInputContext& /* ctx */)
{}

void SdlSceneFileReader::storeNamedDataPacket(
	std::string_view packetName,
	const SdlInputClauses& packet,
	const SdlInputContext& ctx)
{
	m_namedDataPackets.addOrUpdate(packet, packetName);
}

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
			"Unable to read scene <{}> (working directory <{}>): no target scene description was set.",
			m_sceneFile, getSceneWorkingDirectory());
		return;
	}

	// Clear existing named packets before reading new file
	m_namedDataPackets = SdlDataPacketCollection();

	FormattedTextInputStream commandFile(m_sceneFile);
	if(!commandFile)
	{
		PH_LOG_WARNING(SdlSceneFileReader,
			"command file <{}> opening failed", m_sceneFile.toAbsoluteString());
		return;
	}
	else
	{
		PH_LOG(SdlSceneFileReader, 
			"loading command file <{}>", m_sceneFile.toAbsoluteString());

		Timer timer;
		timer.start();

		// TODO: can just read all then parse all
		std::string lineOfCommand;
		while(commandFile)
		{
			commandFile.readLine(&lineOfCommand);
			lineOfCommand += '\n';

			parse(lineOfCommand);
		}
		flush();

		timer.stop();

		PH_LOG(SdlSceneFileReader, 
			"command file PSDL version: {}", getCommandVersion().toString());
		PH_LOG(SdlSceneFileReader, 
			"command file loaded, time elapsed = {} ms", timer.getDeltaMs());
	}
}

void SdlSceneFileReader::setSceneFile(Path sceneFile)
{
	m_sceneFile = std::move(sceneFile);
}

void SdlSceneFileReader::setScene(SceneDescription* const scene)
{
	m_scene = scene;
}

}// end namespace ph
