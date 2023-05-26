#include "SDL/SdlCommandGenerator.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/ISdlResource.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/SdlOutputClauses.h"
#include "Common/assertion.h"
#include "Common/logging.h"

#include <cstddef>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlCommandGenerator, SDL);

SdlCommandGenerator::SdlCommandGenerator()
	: SdlCommandGenerator(Path("./"))
{}

SdlCommandGenerator::SdlCommandGenerator(const Path& sceneWorkingDirectory)
	: m_sceneWorkingDirectory(sceneWorkingDirectory)
	, m_inlinePacketGenerator(sceneWorkingDirectory)
	, m_numGeneratedCommands(0)
	, m_numGenerationErrors(0)
{}

SdlCommandGenerator::~SdlCommandGenerator() = default;

SdlDataPacketGenerator& SdlCommandGenerator::getPacketGenerator()
{
	return m_inlinePacketGenerator;
}

void SdlCommandGenerator::generateLoadCommand(
	const ISdlResource* const resource,
	std::string_view resourceName)
{
	if(!resource)
	{
		return;
	}

	const SdlClass* clazz = resource->getDynamicSdlClass();
	PH_ASSERT(clazz);

	try
	{
		if(!beginCommand(clazz))
		{
			return;
		}

		// TODO: reuse clause buffer
		SdlOutputClauses clauses;
		saveResource(resource, clazz, clauses);

		// TODO: reuse string buffer
		std::string generatedCommand;
		generateLoadCommand(
			*resource, 
			clazz,
			resourceName,
			clauses,
			generatedCommand);

		if(!generatedCommand.empty())
		{
			commandGenerated(generatedCommand);
			++m_numGeneratedCommands;
		}
		
		endCommand();
	}
	catch(const SdlSaveError& e)
	{
		std::string resourceNameInfo(resourceName);
		if(resourceNameInfo.empty())
		{
			resourceNameInfo = "(unavailable)";
		}

		PH_LOG_WARNING(SdlCommandGenerator, 
			"error generating load command for resource {} ({}) -> {}",
			resourceNameInfo, clazz->genPrettyName(), e.whatStr());

		++m_numGenerationErrors;
	}
}

void SdlCommandGenerator::setSceneWorkingDirectory(Path directory)
{
	m_sceneWorkingDirectory = std::move(directory);
}

void SdlCommandGenerator::clearStats()
{
	m_numGeneratedCommands = 0;
	m_numGenerationErrors = 0;
}

void SdlCommandGenerator::generateLoadCommand(
	const ISdlResource& resource,
	const SdlClass* const resourceClass,
	std::string_view resourceName,
	const SdlOutputClauses& clauses,
	std::string& out_commandStr)
{
	PH_ASSERT(resource.getDynamicSdlClass() == resourceClass);

	appendFullSdlType(resourceClass, out_commandStr);
	out_commandStr += ' ';
	out_commandStr += resourceName;
	out_commandStr += " = ";

	getPacketGenerator().generate(clauses, out_commandStr);

	out_commandStr += '\n';
}

void SdlCommandGenerator::appendFullSdlType(
	const SdlClass* clazz,
	std::string& out_commandStr)
{
	PH_ASSERT(clazz);

	out_commandStr += sdl::category_to_string(clazz->getCategory());
	out_commandStr += '(';
	out_commandStr += clazz->getTypeName();
	out_commandStr += ')';
}

}// end namespace ph
